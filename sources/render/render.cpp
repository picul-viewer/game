#include "render.h"

#include <lib/fixed_string.h>
#include <math/matrix.h>
#include <resources/resources_path.h>
#include <ui/font_cook.h>

#include "dx.h"
#include "parameters.h"
#include "parameters_manager.h"
#include "resources.h"
#include "scene.h"
#include "render_object_mesh.h"
#include "statistics.h"

#include "gen_arg_mesh_effect_cook.h"
#include "render_shadowmap_effect_cook.h"
#include "render_mesh_effect_cook.h"
#include "shade_effect_cook.h"
#include "resolve_effect_cook.h"
#include "render_ui_effect_cook.h"

namespace render {

void render::create( )
{
	m_ready = false;
	m_copy_finished = true;
	m_need_to_record_render = false;

	m_scene = nullptr;
	m_next_scene = nullptr;

	g_dx.create( );

	g_gpu_uploader.create( );

	g_resources.create( );

	m_ui_processor.create( );

#ifdef USE_RENDER_PROFILING
	g_statistics.create( );
#endif // #ifdef USE_RENDER_PROFILING

	{
		m_frame_index = 0;

		m_gpu_frame_index.create( 0 );
		set_dx_name( m_gpu_frame_index, "gpu_frame_event" );

		m_gpu_frame_event.create( false, false );
	}

	m_rs_queue_event = resource_system::queue_event( engine_thread_main );

	{
		dx_command_signature::indirect_argument argument;
		argument.create_dispatch( );

		dx_command_signature::cook cook;
		cook.create( sizeof(gpu::dispatch_indirect_command), 1, &argument );

		m_dispatch_cs.create( cook, dx_root_signature( ) );
		set_dx_name( m_dispatch_cs, "dispatch_cs" );
	}

	{
		dx_command_signature::indirect_argument argument;
		argument.create_draw( );

		dx_command_signature::cook cook;
		cook.create( sizeof(gpu::draw_indirect_command), 1, &argument );

		m_draw_cs.create( cook, dx_root_signature( ) );
		set_dx_name( m_dispatch_cs, "draw_cs" );
	}

	{
		dx_command_signature::indirect_argument argument;
		argument.create_draw_indexed( );

		dx_command_signature::cook cook;
		cook.create( sizeof(gpu::draw_indexed_indirect_command), 1, &argument );

		m_draw_indexed_cs.create( cook, dx_root_signature( ) );
		set_dx_name( m_dispatch_cs, "draw_indexed_cs" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			max_mesh_object_list_size * sizeof(u32),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_mesh_object_list[i].create( cook );
			set_dx_name( m_mesh_object_list[i], format( "mesh_object_list #%d", i ) );
			m_sun_shadowmap_mesh_object_list[i].create( cook );
			set_dx_name( m_sun_shadowmap_mesh_object_list[i], format( "sun_shadowmap_mesh_object_list #%d", i ) );
		}
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			max_point_light_object_list_size * sizeof(u32),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_point_light_object_list[i].create( cook );
			set_dx_name( m_point_light_object_list[i], format( "point_light_object_list #%d", i ) );
		}
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			max_mesh_object_list_size * sizeof(gpu::draw_indexed_indirect_command),
			false, true, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT );

		m_mesh_indirect_arguments_buffer.create( cook );
		set_dx_name( m_mesh_indirect_arguments_buffer, "mesh_indirect_arguments_buffer" );
		m_sun_shadowmap_indirect_arguments_buffer.create( cook );
		set_dx_name( m_sun_shadowmap_indirect_arguments_buffer, "sun_shadowmap_indirect_arguments_buffer" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			max_mesh_object_list_size * sizeof(math::float4x4),
			true, true, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER );

		m_mesh_transforms_buffer.create( cook );
		set_dx_name( m_mesh_transforms_buffer, "mesh_transforms_buffer" );
		m_sun_shadowmap_mesh_transforms_buffer.create( cook );
		set_dx_name( m_sun_shadowmap_mesh_transforms_buffer, "sun_shadowmap_mesh_transforms_buffer" );
	}

	m_debug_font.reset( );
	u32 const max_task_count = 20;
	task_info tasks_memory[max_task_count];
	lib::buffer_array<task_info> tasks( tasks_memory, max_task_count );

	// Create font for statistics
	{
		ui::font_cook* const cook = ui::font_cook::create( GET_RESOURCE_PATH( "configs\\fonts\\console.font.cfg" ) );
		cook->fill_task_info( tasks.emplace_back( ) );
	}

	fill_effect_tasks( tasks );

	resource_system::create_resources(
		tasks.data( ), (u32)tasks.size( ),
		resource_system::user_callback_task<render, &render::on_resources_created>( this )
	);
}

void render::on_resources_created( queried_resources& in_resources )
{
	m_debug_font = in_resources.get_resource<ui::font::ptr>( );

	for ( u32 i = 1; i < in_resources.count( ); ++i )
	{
		pipeline_state* result = in_resources.get_resource<pipeline_state*>( );
		ASSERT( result );
	}

	record_render( );

	m_ready = true;
}

void render::fill_effect_tasks( lib::buffer_array<task_info>& in_tasks )
{
	{
		gen_arg_mesh_effect_cook* const cook = gen_arg_mesh_effect_cook::create( &m_ps_gen_arg_mesh, gen_arg_mesh_effect_cook::type_mesh );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		gen_arg_mesh_effect_cook* const cook = gen_arg_mesh_effect_cook::create( &m_ps_gen_arg_sun_shadowmap, gen_arg_mesh_effect_cook::type_sun_shadowmap );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		render_shadowmap_effect_cook* const cook = render_shadowmap_effect_cook::create( &m_ps_render_shadowmap_directional, true );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		render_shadowmap_effect_cook* const cook = render_shadowmap_effect_cook::create( &m_ps_render_shadowmap, false );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		render_mesh_effect_cook* const cook = render_mesh_effect_cook::create( &m_ps_render_mesh );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shade_effect_cook* const cook = shade_effect_cook::create( &m_ps_shade );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		resolve_effect_cook* const cook = resolve_effect_cook::create( &m_ps_resolve );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		render_ui_effect_cook* const cook = render_ui_effect_cook::create( &m_ps_render_ui );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}
}

void render::destroy_effects( )
{
	m_ps_gen_arg_mesh.destroy( );
	m_ps_gen_arg_sun_shadowmap.destroy( );
	m_ps_render_shadowmap_directional.destroy( );
	m_ps_render_shadowmap.destroy( );
	m_ps_render_mesh.destroy( );
	m_ps_shade.destroy( );
	m_ps_resolve.destroy( );
	m_ps_render_ui.destroy( );
}

void render::record_render( )
{
	m_cmd_allocator.create( D3D12_COMMAND_LIST_TYPE_DIRECT );
	m_cmd_allocator->Reset( );
	set_dx_name( m_cmd_allocator, "cmd_allocator" );

	for ( u32 i = 0; i < max_frame_delay; ++i )
	{
		for ( u32 j = 0; j < max_frame_delay; ++j )
		{
			dx_command_list& list = m_cmd_lists[i][j];
			list.create( D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmd_allocator, nullptr );
			set_dx_name( list, format( "cmd_list[%d][%d]", i, j ) );

			{
				PROFILE_EVENT( list, j, Frame );

				{
					D3D12_RESOURCE_BARRIER barriers[4];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = m_mesh_indirect_arguments_buffer;
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

					barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[1].Transition.pResource = m_mesh_transforms_buffer;
					barriers[1].Transition.Subresource = 0;
					barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
					barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

					barriers[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[2].Transition.pResource = m_sun_shadowmap_indirect_arguments_buffer;
					barriers[2].Transition.Subresource = 0;
					barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
					barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

					barriers[3].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[3].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[3].Transition.pResource = m_sun_shadowmap_mesh_transforms_buffer;
					barriers[3].Transition.Subresource = 0;
					barriers[3].Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
					barriers[3].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, gen_arg_mesh );

					list->SetComputeRootSignature( m_ps_gen_arg_mesh.rs( ) );
					list->SetPipelineState( m_ps_gen_arg_mesh.ps( ) );

					list->SetComputeRootConstantBufferView( 0, g_resources.constant_buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 1, m_mesh_object_list[j]->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 2, g_resources.mesh_object_buffer( )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 3, g_resources.transforms( ).buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootUnorderedAccessView( 4, m_mesh_indirect_arguments_buffer->GetGPUVirtualAddress( ) );
					list->SetComputeRootUnorderedAccessView( 5, m_mesh_transforms_buffer->GetGPUVirtualAddress( ) );

					u32 const dispatch_size_offset = offsetof( gpu::constant_buffer, indirect_params_1.x );
					list->ExecuteIndirect( m_dispatch_cs, 1, g_resources.constant_buffer( j ), dispatch_size_offset, nullptr, 0 );
				}

				{
					PROFILE_EVENT( list, j, gen_arg_sun_shadowmap );

					list->SetComputeRootSignature( m_ps_gen_arg_sun_shadowmap.rs( ) );
					list->SetPipelineState( m_ps_gen_arg_sun_shadowmap.ps( ) );

					list->SetComputeRootConstantBufferView( 0, g_resources.constant_buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 1, m_sun_shadowmap_mesh_object_list[j]->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 2, g_resources.mesh_object_buffer( )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 3, g_resources.transforms( ).buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootUnorderedAccessView( 4, m_sun_shadowmap_indirect_arguments_buffer->GetGPUVirtualAddress( ) );
					list->SetComputeRootUnorderedAccessView( 5, m_sun_shadowmap_mesh_transforms_buffer->GetGPUVirtualAddress( ) );

					u32 const dispatch_size_offset = offsetof( gpu::constant_buffer, indirect_params_2.x );
					list->ExecuteIndirect( m_dispatch_cs, 1, g_resources.constant_buffer( j ), dispatch_size_offset, nullptr, 0 );
				}

				{
					D3D12_RESOURCE_BARRIER barriers[6];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = m_mesh_indirect_arguments_buffer;
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;

					barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[1].Transition.pResource = m_mesh_transforms_buffer;
					barriers[1].Transition.Subresource = 0;
					barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

					barriers[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[2].Transition.pResource = m_sun_shadowmap_indirect_arguments_buffer;
					barriers[2].Transition.Subresource = 0;
					barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;

					barriers[3].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[3].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[3].Transition.pResource = m_sun_shadowmap_mesh_transforms_buffer;
					barriers[3].Transition.Subresource = 0;
					barriers[3].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					barriers[3].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

					barriers[4].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[4].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[4].Transition.pResource = g_resources.image( image_v_buffer_polygon_id );
					barriers[4].Transition.Subresource = 0;
					barriers[4].Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
					barriers[4].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

					barriers[5].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[5].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[5].Transition.pResource = g_resources.image( image_sun_shadowmap );
					barriers[5].Transition.Subresource = 0;
					barriers[5].Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
					barriers[5].Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, render_mesh );

					list.cmd_set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					list->SetGraphicsRootSignature( m_ps_render_mesh.rs( ) );
					list->SetPipelineState( m_ps_render_mesh.ps( ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const rtv = g_resources.rtv( image_rtv_v_buffer_polygon_id );
					D3D12_CPU_DESCRIPTOR_HANDLE const dsv = g_resources.dsv( image_dsv_screen );
					list->OMSetRenderTargets( 1, &rtv, FALSE, &dsv );

					list->ClearRenderTargetView( rtv, math::float4( 0.0f ).data, 0, nullptr );
					list->ClearDepthStencilView( dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );

					D3D12_INDEX_BUFFER_VIEW const& ibv = g_resources.index_buffer_view( );
					list->IASetIndexBuffer( &ibv );

					D3D12_VERTEX_BUFFER_VIEW vbvs[3];
					vbvs[0] = g_resources.vertex_buffer_view( );
					vbvs[1].BufferLocation = m_mesh_transforms_buffer->GetGPUVirtualAddress( );
					vbvs[1].SizeInBytes = max_mesh_object_list_size * sizeof(math::float4x4);
					vbvs[1].StrideInBytes = sizeof(math::float4x4);
					vbvs[2].BufferLocation = m_mesh_object_list[j]->GetGPUVirtualAddress( );
					vbvs[2].SizeInBytes = max_mesh_object_list_size * sizeof(u32);
					vbvs[2].StrideInBytes = sizeof(u32);
					list->IASetVertexBuffers( 0, (u32)array_size( vbvs ), vbvs );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_0.x );
					list->ExecuteIndirect(
						m_draw_indexed_cs,
						max_mesh_object_list_size,
						m_mesh_indirect_arguments_buffer, 0,
						g_resources.constant_buffer( j ), instance_count_offset
					);
				}

				{
					PROFILE_EVENT( list, j, render_sun_shadowmap );

					list.cmd_set_viewport_and_scissors( math::u32x2( resources::sun_shadowmap_dimension ) );

					list->SetGraphicsRootSignature( m_ps_render_shadowmap_directional.rs( ) );
					list->SetPipelineState( m_ps_render_shadowmap_directional.ps( ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const dsv = g_resources.dsv( image_dsv_sun_shadowmap );
					list->OMSetRenderTargets( 0, nullptr, FALSE, &dsv );

					list->ClearDepthStencilView( dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );

					D3D12_INDEX_BUFFER_VIEW const& ibv = g_resources.index_buffer_view( );
					list->IASetIndexBuffer( &ibv );

					D3D12_VERTEX_BUFFER_VIEW vbvs[2];
					vbvs[0] = g_resources.vertex_buffer_view( );
					vbvs[1].BufferLocation = m_sun_shadowmap_mesh_transforms_buffer->GetGPUVirtualAddress( );
					vbvs[1].SizeInBytes = max_mesh_object_list_size * sizeof(math::float4x4);
					vbvs[1].StrideInBytes = sizeof(math::float4x4);
					list->IASetVertexBuffers( 0, (u32)array_size( vbvs ), vbvs );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_0.y );
					list->ExecuteIndirect(
						m_draw_indexed_cs,
						max_mesh_object_list_size,
						m_sun_shadowmap_indirect_arguments_buffer, 0,
						g_resources.constant_buffer( j ), instance_count_offset
					);
				}

				{
					D3D12_RESOURCE_BARRIER barriers[5];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = g_resources.image( image_v_buffer_polygon_id );
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

					barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[1].Transition.pResource = g_resources.index_buffer( );
					barriers[1].Transition.Subresource = 0;
					barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_INDEX_BUFFER;
					barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

					barriers[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[2].Transition.pResource = g_resources.vertex_buffer( );
					barriers[2].Transition.Subresource = 0;
					barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
					barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

					barriers[3].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[3].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[3].Transition.pResource = g_dx.swap_chain_buffer( i );
					barriers[3].Transition.Subresource = 0;
					barriers[3].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
					barriers[3].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

					barriers[4].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[4].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[4].Transition.pResource = g_resources.image( image_sun_shadowmap );
					barriers[4].Transition.Subresource = 0;
					barriers[4].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
					barriers[4].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, shade );

					list->SetComputeRootSignature( m_ps_shade.rs( ) );
					list->SetPipelineState( m_ps_shade.ps( ) );

					ID3D12DescriptorHeap* const heap = g_resources.srv_uav_heap( );
					list->SetDescriptorHeaps( 1, &heap );

					list->SetComputeRootConstantBufferView( 0, g_resources.constant_buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 1, g_resources.mesh_object_buffer( )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 2, g_resources.index_buffer( )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 3, g_resources.vertex_buffer( )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 4, g_resources.vertex_data_buffer( )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 5, g_resources.transforms( ).buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 6, g_resources.point_lights( ).buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 7, m_point_light_object_list[j]->GetGPUVirtualAddress( ) );
					list->SetComputeRootDescriptorTable( 8, g_resources.srv_uav_heap( )->GetGPUDescriptorHandleForHeapStart( ) );

					u16 const tile_size = 16;
					math::u16x2 const group_count = ( g_parameters.screen_resolution + math::u16x2( tile_size - 1 ) ) / tile_size;
					list->Dispatch( group_count.x, group_count.y, 1 );
				}

				{
					D3D12_RESOURCE_BARRIER barriers[1];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = g_resources.image( image_radiance );
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, resolve );

					list.cmd_set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					list->SetGraphicsRootSignature( m_ps_resolve.rs( ) );
					list->SetPipelineState( m_ps_resolve.ps( ) );

					ID3D12DescriptorHeap* const heap = g_resources.srv_uav_heap( );
					list->SetDescriptorHeaps( 1, &heap );

					list->SetGraphicsRootDescriptorTable( 0, g_resources.srv_uav_heap( )->GetGPUDescriptorHandleForHeapStart( ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const rtv = g_resources.rtv( image_rtv_output_0 + i );
					list->OMSetRenderTargets( 1, &rtv, TRUE, nullptr );

					list->DrawInstanced( 3, 1, 0, 0 );
				}

				{
					D3D12_RESOURCE_BARRIER barriers[1];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = g_resources.image( image_radiance );
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, render_ui );

					list.cmd_set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					list->SetGraphicsRootSignature( m_ps_render_ui.rs( ) );
					list->SetPipelineState( m_ps_render_ui.ps( ) );

					ID3D12DescriptorHeap* const heap = g_resources.srv_uav_heap( );
					list->SetDescriptorHeaps( 1, &heap );

					list->SetGraphicsRootDescriptorTable( 0, g_resources.srv_uav_heap( )->GetGPUDescriptorHandleForHeapStart( ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const rtv = g_resources.rtv( image_rtv_output_0 + i );
					list->OMSetRenderTargets( 1, &rtv, FALSE, nullptr );

					D3D12_VERTEX_BUFFER_VIEW const& vbv = m_ui_processor.vertex_buffer_view( j );
					list->IASetVertexBuffers( 0, 1, &vbv );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_0.w );
					list->ExecuteIndirect(
						m_draw_cs,
						ui_processor::max_batches,
						m_ui_processor.command_buffer( j ), 0,
						g_resources.constant_buffer( j ), instance_count_offset
					);
				}

				{
					D3D12_RESOURCE_BARRIER barriers[1];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = g_dx.swap_chain_buffer( i );
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}
			}

#ifdef USE_RENDER_PROFILING
			g_statistics.resolve( list, j );
#endif // #ifdef USE_RENDER_PROFILING

			DX12_CHECK_RESULT( list->Close( ) );
		}
	}
}

void render::destroy( )
{
	m_ready = false;

	// Wait for GPU.
	render_wait( [this]( ) { return m_gpu_frame_index->GetCompletedValue( ) >= m_frame_index; } );

	m_mesh_indirect_arguments_buffer.destroy( );
	m_mesh_transforms_buffer.destroy( );
	m_sun_shadowmap_indirect_arguments_buffer.destroy( );
	m_sun_shadowmap_mesh_transforms_buffer.destroy( );

	destroy_effects( );

	resource_system::destroy_resources(
		resource_system::user_callback_task<render, &render::on_resources_destroyed>( this ),
		m_debug_font.get( )
	);

	m_debug_font.reset( );
}

void render::on_resources_destroyed( )
{
	m_gpu_frame_index.destroy( );
	m_gpu_frame_event.destroy( );

	for ( u32 i = 0 ; i < max_frame_delay; ++i )
		for ( u32 j = 0 ; j < max_frame_delay; ++j )
			m_cmd_lists[i][j].destroy( );
	m_cmd_allocator.destroy( );

	for ( u32 i = 0 ; i < max_frame_delay; ++i )
	{
		m_mesh_object_list[i].destroy( );
		m_point_light_object_list[i].destroy( );
	}

	m_dispatch_cs.destroy( );
	m_draw_cs.destroy( );
	m_draw_indexed_cs.destroy( );

#ifdef USE_RENDER_PROFILING
	g_statistics.destroy( );
#endif // #ifdef USE_RENDER_PROFILING

	m_ui_processor.destroy( );

	g_resources.destroy( );

	g_gpu_uploader.destroy( );

	g_dx.destroy( );

	m_ready = true;
}

bool render::ready( )
{
	return m_ready;
}

void render::reload_render( )
{
	m_ready = false;

	for ( u32 i = 0; i < max_frame_delay; ++i )
		for ( u32 j = 0; j < max_frame_delay; ++j )
			m_cmd_lists[i][j].destroy( );
	m_cmd_allocator.destroy( );

	destroy_effects( );

	u32 const max_task_count = 20;
	task_info tasks_memory[max_task_count];
	lib::buffer_array<task_info> tasks( tasks_memory, max_task_count );

	fill_effect_tasks( tasks );

	resource_system::create_resources(
		tasks.data( ), (u32)tasks.size( ),
		resource_system::user_callback_task<render, &render::on_effects_created>( this )
	);
}

void render::on_effects_created( )
{
	record_render( );

	m_ready = true;

	// To exit from potential wait.
	m_rs_queue_event.set( );
}

template<typename Functor>
void render::render_wait( Functor const& in_functor )
{
	sys::system_event const events[] = { m_gpu_frame_event, m_rs_queue_event };

	while ( !in_functor( ) )
	{
		bool const rs_queue_processed = resource_system::process_task( engine_thread_main );

		if ( rs_queue_processed )
			continue;

		sys::system_event::wait_any( (u32)array_size( events ), events );
	}
}

void render::push_cmd_lists( )
{
	u32 const swap_chain_buffer_index = g_dx.swap_chain( )->GetCurrentBackBufferIndex( );
	u32 const frame_list_index = ( m_frame_index + max_frame_delay - 1 ) % max_frame_delay;
	ID3D12CommandList* const current_cmd_list = m_cmd_lists[swap_chain_buffer_index][frame_list_index];
	g_dx.queue_graphics( )->ExecuteCommandLists( 1, &current_cmd_list );
}

void render::update( )
{
	// Wait for copy tasks for current frame.
	if ( !m_need_to_record_render )
	{
		render_wait( [this]( ) { return m_copy_finished; } );

		// Push GPU commands for current frame.
		push_cmd_lists( );

		// Wait for previous frame.
		render_wait( [this]( ) { return m_gpu_frame_index->GetCompletedValue( ) >= m_frame_index; } );
	}
	else
	{
		render_wait( [this]( ) { return m_copy_finished && ( m_gpu_frame_index->GetCompletedValue( ) >= m_frame_index ); } );

		reload_render( );
		render_wait( [this]( ) { return ready( ); } );
		// This was set, but might not be reset (if WaitForMultipleObjects wasn't called).
		m_rs_queue_event.reset( );
		m_need_to_record_render = false;

		// Push GPU commands for current frame.
		push_cmd_lists( );
	}

	// Present (previous frame).
	DX12_CHECK_RESULT( g_dx.swap_chain( )->Present( 0, 0 ) );

	DX12_CHECK_RESULT( g_dx.queue_graphics( )->Signal( m_gpu_frame_index, m_frame_index + 1 ) );
	m_gpu_frame_index->SetEventOnCompletion( m_frame_index + 1, m_gpu_frame_event.get_handle( ) );

	// Prepeare next frame on CPU.
	m_scene = m_next_scene;
	g_parameters_manager.update( );

	prepare_frame( );

	++m_frame_index;
}

void render::prepare_frame( )
{
	m_copy_finished = false;

	u32 const cmd_index = m_frame_index % max_frame_delay;

	// Initialize render camera.
	m_render_camera.set_view( g_parameters.camera.view );
	m_render_camera.set_perspective( g_parameters.camera.fov, (float)g_parameters.screen_resolution.x / (float)g_parameters.screen_resolution.y, 0.1f, 1000.0f );
	m_render_camera.update( );

	u32 copy_task_count = 1; // Constant buffer.
	copy_task_count += m_ui_processor.upload_task_count_needed( );
	copy_task_count = m_scene											? copy_task_count + 3 : copy_task_count;
	copy_task_count = g_resources.transforms( ).need_gpu_update( )		? copy_task_count + 1 : copy_task_count;
	copy_task_count = g_resources.point_lights( ).need_gpu_update( )	? copy_task_count + 1 : copy_task_count;

	ASSERT_CMP( copy_task_count, <=, max_copy_task_count );

	custom_task_context const context = resource_system::create_custom_tasks( copy_task_count, resource_system::user_callback_task<render, &render::prepare_frame_copy>( this ) );
	lib::buffer_array<gpu_upload_task> copy_tasks( m_copy_tasks, copy_task_count );

	u32 mesh_list_size = 0;
	u32 point_light_list_size = 0;
	u32 sun_shadowmap_mesh_list_size = 0;

	if ( m_scene )
	{
		// Filter objects to render.
		math::bvh const& static_mesh_container = m_scene->static_mesh_container( );
		math::bvh const& dynamic_mesh_container = m_scene->dynamic_mesh_container( );

		math::frustum frustum;
		frustum.set_from_matrix( m_render_camera.get_view_projection( ) );

		lib::buffer_array<u32> cpu_mesh_object_list( m_cpu_mesh_object_lists[cmd_index], max_mesh_object_list_size );

		static_mesh_container.frustum_test( frustum, [&cpu_mesh_object_list]( math::bvh::object_handle const in_handle )
		{
			render_object_mesh* const obj = g_resources.render_allocator( ).offset_to_ptr( in_handle );
			cpu_mesh_object_list.push_back( obj->m_object_handle + 1 );
		} );

		dynamic_mesh_container.frustum_test( frustum, [&cpu_mesh_object_list]( math::bvh::object_handle const in_handle )
		{
			render_object_mesh* const obj = g_resources.render_allocator( ).offset_to_ptr( in_handle );
			cpu_mesh_object_list.push_back( obj->m_object_handle + 1 );
		} );

		mesh_list_size = (u32)cpu_mesh_object_list.size( );

		gpu_upload_task& mesh_list_task = copy_tasks.emplace_back( );
		mesh_list_task.set_buffer_upload( m_mesh_object_list[cmd_index], 0 );
		mesh_list_task.set_source_data( cpu_mesh_object_list.data( ), mesh_list_size * sizeof(u32) );


		lib::buffer_array<u32> cpu_sun_shadowmap_mesh_object_list( m_cpu_sun_shadowmap_mesh_object_lists[cmd_index], max_mesh_object_list_size );

		static_mesh_container.for_each( [&cpu_sun_shadowmap_mesh_object_list]( math::bvh::object_handle const in_handle )
		{
			render_object_mesh* const obj = g_resources.render_allocator( ).offset_to_ptr( in_handle );
			cpu_sun_shadowmap_mesh_object_list.push_back( obj->m_object_handle + 1 );
		} );

		dynamic_mesh_container.for_each( [&cpu_sun_shadowmap_mesh_object_list]( math::bvh::object_handle const in_handle )
		{
			render_object_mesh* const obj = g_resources.render_allocator( ).offset_to_ptr( in_handle );
			cpu_sun_shadowmap_mesh_object_list.push_back( obj->m_object_handle + 1 );
		} );

		sun_shadowmap_mesh_list_size = (u32)cpu_sun_shadowmap_mesh_object_list.size( );

		gpu_upload_task& sun_shadowmap_mesh_list_task = copy_tasks.emplace_back( );
		sun_shadowmap_mesh_list_task.set_buffer_upload( m_sun_shadowmap_mesh_object_list[cmd_index], 0 );
		sun_shadowmap_mesh_list_task.set_source_data( cpu_sun_shadowmap_mesh_object_list.data( ), sun_shadowmap_mesh_list_size * sizeof(u32) );


		auto const& static_point_light_container = m_scene->static_point_light_container( );

		lib::buffer_array<u32> cpu_point_light_object_list( m_cpu_point_light_object_lists[cmd_index], max_point_light_object_list_size );

		for ( u32 i = 0; i < static_point_light_container.size( ); ++i )
		{
			math::float4 const& sphere = math::float4( static_point_light_container[i].m_data.position, static_point_light_container[i].m_data.range );
			if ( frustum.test_sphere_outside( sphere ) )
				continue;

			cpu_point_light_object_list.push_back( static_point_light_container[i].m_object_handle );
		}

		point_light_list_size = (u32)cpu_point_light_object_list.size( );

		gpu_upload_task& point_light_list_task = copy_tasks.emplace_back( );
		point_light_list_task.set_buffer_upload( m_point_light_object_list[cmd_index], 0 );
		point_light_list_task.set_source_data( cpu_point_light_object_list.data( ), point_light_list_size * sizeof(u32) );
	}

	// Process UI.
	u32 const ui_dispatch_dimension = m_ui_processor.prepare_ui( copy_tasks );

	// Fill constant buffers.
	gpu::constant_buffer& cpu_constant_buffer = m_cpu_constant_buffers[cmd_index];

	m_render_camera.fill_constant_buffer( cpu_constant_buffer );
	cpu_constant_buffer.viewport_size = math::float4( g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, 1.0f / g_parameters.screen_resolution.x, 1.0f / g_parameters.screen_resolution.y );

	if ( m_scene )
	{
		cpu_constant_buffer.sun_direction.vx = m_scene->get_sun_direction( );
		cpu_constant_buffer.sun_radiance.vx = m_scene->get_sun_radiance( );
		cpu_constant_buffer.sun_radiance.w = ( m_scene->get_sun_radiance( ) == math::float3( 0.0f ) ) ? 0.0f : 1.0f;

		ASSERT( m_scene->get_sun_direction( ).y > 0.0f );
		math::aabb scene_aabb = m_scene->static_mesh_container( ).get_aabb( );
		scene_aabb.m_min -= math::float3( 0.1f );
		scene_aabb.m_max += math::float3( 0.1f );
		math::float3 const& offset = scene_aabb.radius( ).y / m_scene->get_sun_direction( ).y * m_scene->get_sun_direction( );
		math::float3 shadow_origin = scene_aabb.center( ) + offset;
		math::float3 shadow_size = scene_aabb.radius( ) + math::abs( offset );

		math::float4x4 sun_shadow_basis;
		sun_shadow_basis.l0 = math::float4( shadow_size.x, 0.0f, 0.0f, 0.0f );
		sun_shadow_basis.l1 = math::float4( 0.0f, 0.0f, shadow_size.z, 0.0f );
		sun_shadow_basis.l2 = math::float4( -2.0f * offset, 0.0f );
		sun_shadow_basis.l3 = math::float4( shadow_origin, 1.0f );
		cpu_constant_buffer.sun_view_projection = math::inverse( math::transpose( sun_shadow_basis ) );

		// Avoid full matrix multiplication.
		cpu_constant_buffer.sun_view_projection_inv_transp_0 = math::float4( -2.0f * offset, shadow_size.x );
		cpu_constant_buffer.sun_view_projection_inv_transp_1 = math::float4( shadow_origin, shadow_size.z );
	}

	cpu_constant_buffer.indirect_params_0 = math::u32x4( mesh_list_size, sun_shadowmap_mesh_list_size, point_light_list_size, ui_dispatch_dimension );
	cpu_constant_buffer.indirect_params_1 = math::u32x4( ( mesh_list_size + 255 ) / 256, 1, 1, 0 );
	cpu_constant_buffer.indirect_params_2 = math::u32x4( ( sun_shadowmap_mesh_list_size + 255 ) / 256, 1, 1, 0 );

	gpu_upload_task& constants_task = copy_tasks.emplace_back( );
	constants_task.set_buffer_upload( g_resources.constant_buffer( cmd_index ), 0 );
	constants_task.set_source_data( &cpu_constant_buffer, sizeof(gpu::constant_buffer) );

	// Update transforms.
	if ( g_resources.transforms( ).need_gpu_update( ) )
		g_resources.transforms( ).update_gpu_task( copy_tasks.emplace_back( ) );

	// Update point lights.
	if ( g_resources.point_lights( ).need_gpu_update( ) )
		g_resources.point_lights( ).update_gpu_task( copy_tasks.emplace_back( ) );

#ifdef USE_RENDER_PROFILING
	process_statistics( );
#endif // #ifdef USE_RENDER_PROFILING

	ASSERT_CMP( copy_tasks.size( ), ==, copy_task_count );

	for ( u32 i = 0; i < copy_tasks.size( ); ++i )
		copy_tasks[i].set_task_context( context );

	g_gpu_uploader.push_immediate_tasks( m_copy_tasks, copy_task_count );
}

void render::process_statistics( )
{
	if ( g_parameters.draw_statistics )
	{
		str4096 stat_str;
		g_statistics.process( stat_str.data( ), str4096::max_string_size );

		m_debug_font->render_multiline_string(
			stat_str.c_str( ), stat_str.length( ),
			math::u16x2( 0, 0 ), 0, 0, math::half4( 1.0f, 1.0f, 0.0f, 1.0f ) );
		ui_next_level( );
	}
}

void render::prepare_frame_copy( )
{
	m_copy_finished = true;
	m_gpu_frame_event.set( );
}

void render::ui_add_quad(
	math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
	math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_handle const in_texture )
{
	m_ui_processor.add_quad( in_corners_position, in_corners_texcoord, in_mult_color, in_add_color, in_texture );
}

void render::ui_add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color )
{
	m_ui_processor.add_color_quad( in_corners_position, in_color );
}

void render::ui_next_level( )
{
	m_ui_processor.next_level( );
}

void render::reload( )
{
	m_need_to_record_render = true;
}

render g_render;

} // namespace render
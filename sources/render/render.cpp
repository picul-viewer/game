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

#include "generate_mesh_arguments_effect_cook.h"
#include "render_mesh_objects_effect_cook.h"
#include "shade_effect_cook.h"
#include "render_ui_effect_cook.h"

namespace render {

void render::create( )
{
	m_ready = false;
	m_copy_finished = true;

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
		}
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			render::max_mesh_object_list_size * sizeof(gpu::draw_indexed_indirect_command),
			false, true, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT );

		m_indirect_arguments_buffer.create( cook );
		set_dx_name( m_indirect_arguments_buffer, "indirect_arguments_buffer" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			render::max_mesh_object_list_size * sizeof(math::float4x4),
			true, true, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER );

		m_instance_transforms_buffer.create( cook );
		set_dx_name( m_instance_transforms_buffer, "instance_transforms_buffer" );
	}

	m_debug_font.reset( );

	m_effect_count = 0;

	u32 const task_count = 5;
	task_info tasks[task_count];
	u32 index = 0;

	{
		ui::font_cook* const cook = ui::font_cook::create( GET_RESOURCE_PATH( "configs\\fonts\\console.font.cfg" ) );
		cook->fill_task_info( tasks[index++] );
	}

	{
		generate_mesh_arguments_effect_cook* const cook = generate_mesh_arguments_effect_cook::create( &m_ps_generate_mesh_arguments );
		cook->fill_task_info( tasks[index++] );
		++m_effect_count;
	}

	{
		render_mesh_objects_effect_cook* const cook = render_mesh_objects_effect_cook::create( &m_ps_render_mesh_objects );
		cook->fill_task_info( tasks[index++] );
		++m_effect_count;
	}

	{
		shade_effect_cook* const cook = shade_effect_cook::create( &m_ps_shade );
		cook->fill_task_info( tasks[index++] );
		++m_effect_count;
	}

	{
		render_ui_effect_cook* const cook = render_ui_effect_cook::create( &m_ps_render_ui );
		cook->fill_task_info( tasks[index++] );
		++m_effect_count;
	}

	ASSERT( index == task_count );

	resource_system::create_resources(
		tasks, task_count,
		resource_system::user_callback_task<render, &render::on_resources_created>( this )
	);
}

void render::on_resources_created( queried_resources& in_resources )
{
	m_debug_font = in_resources.get_resource<ui::font::ptr>( );

	for ( uptr i = 0; i < m_effect_count; ++i )
	{
		pipeline_state* result = in_resources.get_resource<pipeline_state*>( );
		ASSERT( result );
	}

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
					D3D12_RESOURCE_BARRIER barriers[2];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = m_indirect_arguments_buffer;
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

					barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[1].Transition.pResource = m_instance_transforms_buffer;
					barriers[1].Transition.Subresource = 0;
					barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
					barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, generate_mesh_arguments );

					list->SetComputeRootSignature( m_ps_generate_mesh_arguments.rs( ) );
					list->SetPipelineState( m_ps_generate_mesh_arguments.ps( ) );

					list->SetComputeRootConstantBufferView( 0, g_resources.constant_buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 1, m_mesh_object_list[j]->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 2, g_resources.mesh_object_buffer( )->GetGPUVirtualAddress( ) );
					list->SetComputeRootShaderResourceView( 3, g_resources.transform_buffer( j )->GetGPUVirtualAddress( ) );
					list->SetComputeRootUnorderedAccessView( 4, m_indirect_arguments_buffer->GetGPUVirtualAddress( ) );
					list->SetComputeRootUnorderedAccessView( 5, m_instance_transforms_buffer->GetGPUVirtualAddress( ) );

					u32 const dispatch_size_offset = offsetof( gpu::constant_buffer, indirect_params_0.x );
					list->ExecuteIndirect( m_dispatch_cs, 1, g_resources.constant_buffer( j ), dispatch_size_offset, nullptr, 0 );
				}

				{
					D3D12_RESOURCE_BARRIER barriers[3];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = m_indirect_arguments_buffer;
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;

					barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[1].Transition.pResource = m_instance_transforms_buffer;
					barriers[1].Transition.Subresource = 0;
					barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

					barriers[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[2].Transition.pResource = g_resources.image( image_v_buffer_polygon_id );
					barriers[2].Transition.Subresource = 0;
					barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
					barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, render_mesh_objects );

					list.cmd_set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					list->SetGraphicsRootSignature( m_ps_render_mesh_objects.rs( ) );
					list->SetPipelineState( m_ps_render_mesh_objects.ps( ) );

					list->OMSetStencilRef( 0x1 );

					D3D12_CPU_DESCRIPTOR_HANDLE const rtv = g_resources.rtv( image_rtv_v_buffer_polygon_id );
					D3D12_CPU_DESCRIPTOR_HANDLE const dsv = g_resources.dsv( );
					list->OMSetRenderTargets( 1, &rtv, FALSE, &dsv );

					list->ClearRenderTargetView( rtv, math::float4( 0.0f ).data, 0, nullptr );
					list->ClearDepthStencilView( dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr );

					D3D12_INDEX_BUFFER_VIEW const& ibv = g_resources.index_buffer_view( );
					list->IASetIndexBuffer( &ibv );

					D3D12_VERTEX_BUFFER_VIEW vbvs[3];
					vbvs[0] = g_resources.vertex_buffer_view( );
					vbvs[1].BufferLocation = m_instance_transforms_buffer->GetGPUVirtualAddress( );
					vbvs[1].SizeInBytes = render::max_mesh_object_list_size * sizeof(math::float4x4);
					vbvs[1].StrideInBytes = sizeof(math::float4x4);
					vbvs[2].BufferLocation = m_mesh_object_list[j]->GetGPUVirtualAddress( );
					vbvs[2].SizeInBytes = render::max_mesh_object_list_size * sizeof(u32);
					vbvs[2].StrideInBytes = sizeof(u32);
					list->IASetVertexBuffers( 0, (u32)array_size( vbvs ), vbvs );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_0.w );
					list->ExecuteIndirect(
						m_draw_indexed_cs,
						render::max_mesh_object_list_size,
						m_indirect_arguments_buffer, 0,
						g_resources.constant_buffer( j ), instance_count_offset
					);
				}

				{
					D3D12_RESOURCE_BARRIER barriers[4];

					barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[0].Transition.pResource = g_resources.image( image_v_buffer_polygon_id );
					barriers[0].Transition.Subresource = 0;
					barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
					barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

					barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[1].Transition.pResource = g_resources.index_buffer( );
					barriers[1].Transition.Subresource = 0;
					barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_INDEX_BUFFER;
					barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

					barriers[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[2].Transition.pResource = g_resources.vertex_buffer( );
					barriers[2].Transition.Subresource = 0;
					barriers[2].Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
					barriers[2].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

					barriers[3].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barriers[3].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barriers[3].Transition.pResource = g_dx.swap_chain_buffer( i );
					barriers[3].Transition.Subresource = 0;
					barriers[3].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
					barriers[3].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

					list->ResourceBarrier( (u32)array_size( barriers ), barriers );
				}

				{
					PROFILE_EVENT( list, j, shade );

					list->SetGraphicsRootSignature( m_ps_shade.rs( ) );
					list->SetPipelineState( m_ps_shade.ps( ) );

					ID3D12DescriptorHeap* const heap = g_resources.srv_heap( );
					list->SetDescriptorHeaps( 1, &heap );

					list->SetGraphicsRootConstantBufferView( 0, g_resources.constant_buffer( j )->GetGPUVirtualAddress( ) );
					list->SetGraphicsRootShaderResourceView( 1, g_resources.mesh_object_buffer( )->GetGPUVirtualAddress( ) );
					list->SetGraphicsRootShaderResourceView( 2, g_resources.index_buffer( )->GetGPUVirtualAddress( ) );
					list->SetGraphicsRootShaderResourceView( 3, g_resources.vertex_buffer( )->GetGPUVirtualAddress( ) );
					list->SetGraphicsRootShaderResourceView( 4, g_resources.vertex_data_buffer( )->GetGPUVirtualAddress( ) );
					list->SetGraphicsRootShaderResourceView( 5, g_resources.transform_buffer( j )->GetGPUVirtualAddress( ) );
					list->SetGraphicsRootDescriptorTable( 6, g_resources.srv_heap( )->GetGPUDescriptorHandleForHeapStart( ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const rtv = g_resources.rtv( image_rtv_output_0 + i );
					D3D12_CPU_DESCRIPTOR_HANDLE const dsv = g_resources.read_only_dsv( );
					list->OMSetRenderTargets( 1, &rtv, TRUE, &dsv );

					list->ClearRenderTargetView( rtv, math::float4( 0.0f ).data, 0, nullptr );

					list->OMSetStencilRef( 0x1 );

					list->DrawInstanced( 3, 1, 0, 0 );
				}

				{
					PROFILE_EVENT( list, j, render_ui );

					list.cmd_set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					list->SetGraphicsRootSignature( m_ps_render_ui.rs( ) );
					list->SetPipelineState( m_ps_render_ui.ps( ) );

					ID3D12DescriptorHeap* const heap = g_resources.srv_heap( );
					list->SetDescriptorHeaps( 1, &heap );

					list->SetGraphicsRootDescriptorTable( 0, g_resources.srv_heap( )->GetGPUDescriptorHandleForHeapStart( ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const rtv = g_resources.rtv( image_rtv_output_0 + i );
					list->OMSetRenderTargets( 1, &rtv, FALSE, nullptr );

					D3D12_VERTEX_BUFFER_VIEW const& vbv = m_ui_processor.vertex_buffer_view( j );
					list->IASetVertexBuffers( 0, 1, &vbv );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_1.x );
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

	m_ready = true;
}

void render::destroy( )
{
	m_ready = false;

	// Wait for GPU.
	{
		sys::system_event const events[] = { m_gpu_frame_event, m_rs_queue_event };

		while ( m_gpu_frame_index->GetCompletedValue( ) < m_frame_index )
		{
			bool const rs_queue_processed = resource_system::process_task( engine_thread_main );

			if ( rs_queue_processed )
				continue;

			sys::system_event::wait_any( (u32)array_size( events ), events );
		}
	}

	m_indirect_arguments_buffer.destroy( );
	m_instance_transforms_buffer.destroy( );

	resource_system::destroy_resources(
		resource_system::user_callback_task<render, &render::on_resources_destroyed>( this ),
		m_debug_font.get( ), &m_ps_generate_mesh_arguments, &m_ps_render_mesh_objects, &m_ps_shade, &m_ps_render_ui
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
		m_mesh_object_list[i].destroy( );

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

void render::update( )
{
	// Wait for copy tasks for current frame.
	{
		sys::system_event const events[] = { m_gpu_frame_event, m_rs_queue_event };

		while ( !m_copy_finished )
		{
			bool const rs_queue_processed = resource_system::process_task( engine_thread_main );

			if ( rs_queue_processed )
				continue;

			sys::system_event::wait_any( (u32)array_size( events ), events );
		}
	}

	// Push GPU commands for current frame.
	{
		u32 const swap_chain_buffer_index = g_dx.swap_chain( )->GetCurrentBackBufferIndex( );
		u32 const frame_list_index = ( m_frame_index + max_frame_delay - 1 ) % max_frame_delay;
		ID3D12CommandList* const current_cmd_list = m_cmd_lists[swap_chain_buffer_index][frame_list_index];
		g_dx.queue_graphics( )->ExecuteCommandLists( 1, &current_cmd_list );
	}

	// Wait for previous frame.
	{
		sys::system_event const events[] = { m_gpu_frame_event, m_rs_queue_event };

		while ( m_gpu_frame_index->GetCompletedValue( ) < m_frame_index )
		{
			bool const rs_queue_processed = resource_system::process_task( engine_thread_main );

			if ( rs_queue_processed )
				continue;

			sys::system_event::wait_any( (u32)array_size( events ), events );
		}
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
	m_render_camera.set_perspective( g_parameters.camera.fov, 1.3333f, 0.01f, 100.0f );
	m_render_camera.update( );

	u32 const ui_upload_tasks_count = m_ui_processor.upload_task_count_needed( );

	u32 copy_task_count = 1 + ui_upload_tasks_count;
	copy_task_count = m_scene										? copy_task_count + 1 : copy_task_count;
	copy_task_count = g_resources.need_dynamic_transforms_update( )	? copy_task_count + 1 : copy_task_count;

	ASSERT_CMP( copy_task_count, <=, max_copy_task_count );

	custom_task_context const context = resource_system::create_custom_tasks( copy_task_count, resource_system::user_callback_task<render, &render::prepare_frame_copy>( this ) );

	u32 task_index = 0;

	u32 mesh_list_size = 0;

	if ( m_scene )
	{
		// Filter objects to render.
		math::bvh const& static_mesh_container = m_scene->static_mesh_container( );
		math::bvh const& dynamic_mesh_container = m_scene->dynamic_mesh_container( );

		math::frustum frustum;
		frustum.set_from_matrix( m_render_camera.get_view_projection( ) );

		lib::buffer_array<u32> cpu_mesh_object_list;
		cpu_mesh_object_list.create( m_cpu_mesh_object_lists[cmd_index], max_mesh_object_list_size );
		cpu_mesh_object_list.clear( );

		static_mesh_container.frustum_test( frustum, [&cpu_mesh_object_list]( math::bvh::object_handle const in_handle )
		{
			render_object_mesh* const obj = g_resources.render_allocator( ).offset_to_ptr( in_handle );
			cpu_mesh_object_list.push_back( obj->m_object_handle );
		} );

		dynamic_mesh_container.frustum_test( frustum, [&cpu_mesh_object_list]( math::bvh::object_handle const in_handle )
		{
			render_object_mesh* const obj = g_resources.render_allocator( ).offset_to_ptr( in_handle );
			cpu_mesh_object_list.push_back( obj->m_object_handle );
		} );

		mesh_list_size = (u32)cpu_mesh_object_list.size( );

		m_copy_tasks[task_index].set_buffer_upload( m_mesh_object_list[cmd_index], 0 );
		m_copy_tasks[task_index].set_source_data( cpu_mesh_object_list.data( ), mesh_list_size * sizeof(u32) );
		m_copy_tasks[task_index].set_task_context( context );
		++task_index;
	}

	// Process UI.
	u32 const ui_dispatch_dimension = m_ui_processor.prepare_ui( m_copy_tasks + task_index );

	for ( u32 i = 0; i < ui_upload_tasks_count; ++i )
		m_copy_tasks[task_index++].set_task_context( context );

	// Fill constant buffers.
	gpu::constant_buffer& cpu_constant_buffer = m_cpu_constant_buffers[cmd_index];

	m_render_camera.fill_constant_buffer( cpu_constant_buffer );
	cpu_constant_buffer.viewport_size = math::float4( g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, 1.0f / g_parameters.screen_resolution.x, 1.0f / g_parameters.screen_resolution.y );
	cpu_constant_buffer.indirect_params_0 = math::u32x4( ( mesh_list_size + 255 ) / 256, 1, 1, mesh_list_size );
	cpu_constant_buffer.indirect_params_1 = math::u32x4( ui_dispatch_dimension, 0, 0, 0 );

	m_copy_tasks[task_index].set_buffer_upload( g_resources.constant_buffer( cmd_index ), 0 );
	m_copy_tasks[task_index].set_source_data( &cpu_constant_buffer, sizeof(gpu::constant_buffer) );
	m_copy_tasks[task_index].set_task_context( context );
	++task_index;

	// Update transforms.
	if ( g_resources.need_dynamic_transforms_update( ) )
	{
		g_resources.update_dynamic_transforms_task( m_copy_tasks[task_index] );
		m_copy_tasks[task_index].set_task_context( context );
		++task_index;
	}

#ifdef USE_RENDER_PROFILING
	process_statistics( );
#endif // #ifdef USE_RENDER_PROFILING

	ASSERT_CMP( task_index, ==, copy_task_count );

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

render g_render;

} // namespace render
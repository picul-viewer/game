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
#include "barrier_builder.h"
#include "command_list_builder.h"
#include "statistics.h"

namespace render {

void render::create( )
{
	m_ready = false;
	m_need_to_record_render = false;

	m_scene = nullptr;
	m_next_scene = nullptr;

	g_dx.create( );

	g_gpu_uploader.create( );

	{
		uptr const upload_buffer_size = 1 * Mb;

		dx_resource::cook cook;
		cook.create_buffer( upload_buffer_size * max_frame_delay, false, false, false, false );
		cook.set_heap_type( D3D12_HEAP_TYPE_UPLOAD );
		cook.set_initial_state( D3D12_RESOURCE_STATE_GENERIC_READ );

		m_copy_staging.create( cook );
		set_dx_name( m_copy_staging, "staging_copy_buffer" );

		D3D12_RANGE const range = { 0, 0 }; // Do not read
		DX12_CHECK_RESULT( m_copy_staging->Map( 0, &range, (pvoid*)&m_copy_staging_data ) );

		m_copy_fence.create( max_frame_delay - 1 );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_copy_cmd_allocators[i].create( D3D12_COMMAND_LIST_TYPE_COPY );
			m_copy_cmd_lists[i].create( D3D12_COMMAND_LIST_TYPE_COPY, m_copy_cmd_allocators[i], nullptr );
			DX12_CHECK_RESULT( m_copy_cmd_lists[i]->Close( ) );
			set_dx_name( m_copy_cmd_lists[i], "copy_cmd_list" );
		}
	}

	g_resources.create( 1024, 64, 64 );

	m_ui_processor.create( );

#ifdef USE_RENDER_PROFILING
	g_statistics.create( );
#endif // #ifdef USE_RENDER_PROFILING

	{
		m_frame_index = max_frame_delay;

		m_gpu_frame_index.create( max_frame_delay - 1 );
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
			max_mesh_list_size * sizeof(u32),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_mesh_list[i].create( cook );
			set_dx_name( m_mesh_list[i], format( "mesh_object_list #%d", i ) );
			m_sun_shadow_mesh_list[i].create( cook );
			set_dx_name( m_sun_shadow_mesh_list[i], format( "sun_shadow_mesh_list #%d", i ) );
		}
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			max_point_light_list_size * sizeof(u32),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_point_light_list[i].create( cook );
			set_dx_name( m_point_light_list[i], format( "point_light_list #%d", i ) );
		}
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			max_mesh_list_size * sizeof(gpu::draw_indexed_indirect_command),
			false, true, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT );

		m_scene_mesh_indirect_buffer.create( cook );
		set_dx_name( m_scene_mesh_indirect_buffer, "scene_mesh_indirect_buffer" );
		m_sun_shadow_mesh_indirect_buffer.create( cook );
		set_dx_name( m_sun_shadow_mesh_indirect_buffer, "sun_shadow_mesh_indirect_buffer" );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			max_mesh_list_size * sizeof(math::float4x4),
			true, true, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER );

		m_scene_mesh_transforms_buffer.create( cook );
		set_dx_name( m_scene_mesh_transforms_buffer, "scene_mesh_transforms_buffer" );
		m_sun_shadow_mesh_transforms_buffer.create( cook );
		set_dx_name( m_sun_shadow_mesh_transforms_buffer, "sun_shadow_mesh_transforms_buffer" );
	}

	// Images.
	{
		m_image_vbuf_polygon_id.create( "image_vbuf_polygon_id", DXGI_FORMAT_R32G32_UINT,
			g_parameters.screen_resolution, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE );
		m_image_radiance.create( "image_radiance", DXGI_FORMAT_R16G16B16A16_FLOAT,
			g_parameters.screen_resolution, D3D12_RESOURCE_STATE_UNORDERED_ACCESS );
		m_image_depth_buffer.create( "image_depth_buffer", DXGI_FORMAT_D32_FLOAT,
			g_parameters.screen_resolution, D3D12_RESOURCE_STATE_DEPTH_WRITE, 1.0f, 0, true, false );
		m_image_sun_shadowmap.create( "image_sun_shadowmap", DXGI_FORMAT_D16_UNORM,
			math::u32x2( sun_shadowmap_dimension ), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, 1.0f, 0, true, false );

		for ( u32 i = 0; i < max_frame_delay; ++i )
			m_backbuffer_rts[i] = g_resources.create_rtv(
				g_dx.swap_chain_buffer( i ), dx_rtv_create_texture2d( g_dx.back_buffer_format ) );
	}

	{
		dx_resource::cook cook;
		cook.create_buffer(
			sizeof(gpu::constant_buffer),
			true, false, false, false
		);
		cook.set_heap_type( D3D12_HEAP_TYPE_DEFAULT );
		cook.set_initial_state( D3D12_RESOURCE_STATE_COMMON );

		for ( u32 i = 0; i < max_frame_delay; ++i )
		{
			m_constant_buffers[i].create( cook );
			set_dx_name( m_constant_buffers[i], format( "constant_buffer #%d", i ) );
		}
	}

	m_debug_font.reset( );
	u32 const max_task_count = 20;
	task_info tasks_memory[max_task_count];
	lib::buffer_array<task_info> tasks( tasks_memory, max_task_count );

	// Create font for statistics
	{
		ui::font_cook* const cook = create_cook<ui::font_cook>( GET_RESOURCE_PATH( "configs\\fonts\\console.font.cfg" ) );
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
		pvoid const result = in_resources.get_resource<pvoid>( );
		ASSERT( result );
	}

	record_render( );

	m_ready = true;
}

void render::fill_effect_tasks( lib::buffer_array<task_info>& in_tasks )
{
	{
		shader_define const define = { "GEN_ARG_TYPE", "0" };
		shader_cook* const shader = create_cook<shader_cook>( shader_type_compute, "gen_arg.cs", 1, &define );

		compute_ps_cook* const cook = create_cook<compute_ps_cook>( &m_ps_gen_arg_mesh, shader );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shader_define const define = { "GEN_ARG_TYPE", "1" };
		shader_cook* const shader = create_cook<shader_cook>(
			shader_type_compute, "gen_arg.cs", 1, &define
		);

		compute_ps_cook* const cook = create_cook<compute_ps_cook>( &m_ps_gen_arg_sun_shadowmap, shader );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shader_define const define = { "DIRECTIONAL_LIGHT", "" };
		shader_cook* const vs_cook = create_cook<shader_cook>(
			shader_type_vertex, "shadowmap.vs", 1, &define
		);

		D3D12_INPUT_ELEMENT_DESC const input_layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
		};

		graphics_ps_cook* const cook = create_cook<graphics_ps_cook>( &m_ps_render_shadowmap_directional );
		cook->set_vs( vs_cook );
		cook->set_depth( true, true, D3D12_COMPARISON_FUNC_LESS_EQUAL );
		cook->set_input_layout( (u32)array_size( input_layout ), input_layout );
		cook->set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
		cook->set_rtv_count( 0 );
		cook->set_dsv_format( DXGI_FORMAT_D16_UNORM );

		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shader_cook* const vs_cook = create_cook<shader_cook>( shader_type_vertex, "shadowmap.vs" );

		D3D12_INPUT_ELEMENT_DESC const input_layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
		};

		graphics_ps_cook* const cook = create_cook<graphics_ps_cook>( &m_ps_render_shadowmap );
		cook->set_vs( vs_cook );
		cook->set_depth( true, true, D3D12_COMPARISON_FUNC_LESS_EQUAL );
		cook->set_input_layout( (u32)array_size( input_layout ), input_layout );
		cook->set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
		cook->set_rtv_count( 0 );
		cook->set_dsv_format( DXGI_FORMAT_D16_UNORM );

		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shader_cook* const vs_cook = create_cook<shader_cook>( shader_type_vertex, "visibility.vs" );
		shader_cook* const ps_cook = create_cook<shader_cook>( shader_type_pixel, "visibility.ps" );

		D3D12_INPUT_ELEMENT_DESC const input_layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE_ID", 0, DXGI_FORMAT_R32_UINT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
		};

		graphics_ps_cook* const cook = create_cook<graphics_ps_cook>( &m_ps_render_mesh );
		cook->set_vs( vs_cook );
		cook->set_ps( ps_cook );
		cook->set_depth( true, true, D3D12_COMPARISON_FUNC_LESS_EQUAL );
		cook->set_input_layout( (u32)array_size( input_layout ), input_layout );
		cook->set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
		cook->set_rtv_count( 1 );
		cook->set_rtv_format( 0, DXGI_FORMAT_R32G32_UINT );
		cook->set_dsv_format( DXGI_FORMAT_D32_FLOAT );

		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shader_define const define = { "SUN_SHADOWMAP_DIMENSION", format( "%d", sun_shadowmap_dimension ) };
		shader_cook* const shader = create_cook<shader_cook>(
			shader_type_compute, "shade.cs", 1, &define
		);

		compute_ps_cook* const cook = create_cook<compute_ps_cook>( &m_ps_shade, shader );
		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shader_cook* const vs_cook = create_cook<shader_cook>( shader_type_vertex, "fullscreen.vs" );
		shader_cook* const ps_cook = create_cook<shader_cook>( shader_type_pixel, "resolve.ps" );

		graphics_ps_cook* const cook = create_cook<graphics_ps_cook>( &m_ps_resolve );
		cook->set_vs( vs_cook );
		cook->set_ps( ps_cook );
		cook->set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
		cook->set_rtv_count( 1 );
		cook->set_rtv_format( 0, g_dx.back_buffer_format );
		cook->set_dsv_format( DXGI_FORMAT_UNKNOWN );

		cook->fill_task_info( in_tasks.emplace_back( ) );
	}

	{
		shader_cook* const vs_cook = create_cook<shader_cook>( shader_type_vertex, "ui.vs" );
		shader_cook* const ps_cook = create_cook<shader_cook>( shader_type_pixel, "ui.ps" );

		D3D12_INPUT_ELEMENT_DESC const input_layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R16G16_SNORM, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 1, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXTURE_ID", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		graphics_ps_cook* const cook = create_cook<graphics_ps_cook>( &m_ps_render_ui );
		cook->set_vs( vs_cook );
		cook->set_ps( ps_cook );
		cook->set_blend_for_rt(
			0, true, false,
			D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ZERO, D3D12_BLEND_ONE, D3D12_BLEND_OP_ADD );
		cook->set_rasterizer_state( D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE );
		cook->set_input_layout( (u32)array_size( input_layout ), input_layout );
		cook->set_primitive_topology( D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE );
		cook->set_rtv_count( 1 );
		cook->set_rtv_format( 0, g_dx.back_buffer_format );
		cook->set_dsv_format( DXGI_FORMAT_UNKNOWN );

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
			dx_command_list& dx_list = m_cmd_lists[i][j];
			dx_list.create( D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmd_allocator, nullptr );
			set_dx_name( dx_list, format( "cmd_list[%d][%d]", i, j ) );
			
			command_list_builder list( dx_list );

			{
				PROFILE_EVENT( list, j, Frame );

				{
					barrier_builder bb( list );
					bb.transition(
						m_scene_mesh_indirect_buffer,
						D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS
					);
					bb.transition(
						m_scene_mesh_transforms_buffer,
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS
					);
					bb.transition(
						m_sun_shadow_mesh_indirect_buffer,
						D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS
					);
					bb.transition(
						m_sun_shadow_mesh_transforms_buffer,
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS
					);
				}

				{
					PROFILE_EVENT( list, j, gen_arg_mesh );

					list.bind_pipeline( m_ps_gen_arg_mesh );

					list.bind_root_cbv( 0, m_constant_buffers[j] );
					list.bind_root_srv( 0, m_mesh_list[j] );
					list.bind_root_srv( 1, g_resources.mesh_object_buffer( ) );
					list.bind_root_srv( 2, g_resources.transforms( ).buffer( j ) );
					list.bind_root_uav( 0, m_scene_mesh_indirect_buffer );
					list.bind_root_uav( 1, m_scene_mesh_transforms_buffer );

					u32 const dispatch_size_offset = offsetof( gpu::constant_buffer, indirect_params_1.x );
					list->ExecuteIndirect( m_dispatch_cs, 1, m_constant_buffers[j], dispatch_size_offset, nullptr, 0 );
				}

				{
					PROFILE_EVENT( list, j, gen_arg_sun_shadowmap );

					list.bind_pipeline( m_ps_gen_arg_sun_shadowmap );

					list.bind_root_cbv( 0, m_constant_buffers[j] );
					list.bind_root_srv( 0, m_sun_shadow_mesh_list[j] );
					list.bind_root_srv( 1, g_resources.mesh_object_buffer( ) );
					list.bind_root_srv( 2, g_resources.transforms( ).buffer( j ) );
					list.bind_root_uav( 0, m_sun_shadow_mesh_indirect_buffer );
					list.bind_root_uav( 1, m_sun_shadow_mesh_transforms_buffer );

					u32 const dispatch_size_offset = offsetof( gpu::constant_buffer, indirect_params_2.x );
					list->ExecuteIndirect( m_dispatch_cs, 1, m_constant_buffers[j], dispatch_size_offset, nullptr, 0 );
				}

				{
					barrier_builder bb( list );
					bb.transition(
						m_scene_mesh_indirect_buffer,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT
					);
					bb.transition(
						m_scene_mesh_transforms_buffer,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
					);
					bb.transition(
						m_sun_shadow_mesh_indirect_buffer,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT
					);
					bb.transition(
						m_sun_shadow_mesh_transforms_buffer,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
					);
					bb.transition(
						m_image_vbuf_polygon_id,
						D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
						D3D12_RESOURCE_STATE_RENDER_TARGET
					);
					bb.transition(
						m_image_sun_shadowmap,
						D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
						D3D12_RESOURCE_STATE_DEPTH_WRITE
					);
				}

				{
					PROFILE_EVENT( list, j, render_mesh );

					list.bind_pipeline( m_ps_render_mesh );
					list.set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const rtv = m_image_vbuf_polygon_id.rtv( );
					D3D12_CPU_DESCRIPTOR_HANDLE const dsv = m_image_depth_buffer.dsv( );
					list->OMSetRenderTargets( 1, &rtv, FALSE, &dsv );

					list->ClearRenderTargetView( rtv, math::float4( 0.0f ).data, 0, nullptr );
					list->ClearDepthStencilView( dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );

					D3D12_INDEX_BUFFER_VIEW const& ibv = g_resources.index_buffer_view( );
					list->IASetIndexBuffer( &ibv );

					D3D12_VERTEX_BUFFER_VIEW vbvs[3];
					vbvs[0] = g_resources.vertex_buffer_view( );
					vbvs[1].BufferLocation = m_scene_mesh_transforms_buffer->GetGPUVirtualAddress( );
					vbvs[1].SizeInBytes = max_mesh_list_size * sizeof(math::float4x4);
					vbvs[1].StrideInBytes = sizeof(math::float4x4);
					vbvs[2].BufferLocation = m_mesh_list[j]->GetGPUVirtualAddress( );
					vbvs[2].SizeInBytes = max_mesh_list_size * sizeof(u32);
					vbvs[2].StrideInBytes = sizeof(u32);
					list->IASetVertexBuffers( 0, (u32)array_size( vbvs ), vbvs );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_0.x );
					list->ExecuteIndirect(
						m_draw_indexed_cs,
						max_mesh_list_size,
						m_scene_mesh_indirect_buffer, 0,
						m_constant_buffers[j], instance_count_offset
					);
				}

				{
					PROFILE_EVENT( list, j, render_sun_shadowmap );

					list.bind_pipeline( m_ps_render_shadowmap_directional );
					list.set_viewport_and_scissors( math::u32x2( sun_shadowmap_dimension ) );

					D3D12_CPU_DESCRIPTOR_HANDLE const dsv = m_image_sun_shadowmap.dsv( );
					list->OMSetRenderTargets( 0, nullptr, FALSE, &dsv );

					list->ClearDepthStencilView( dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );

					D3D12_INDEX_BUFFER_VIEW const& ibv = g_resources.index_buffer_view( );
					list->IASetIndexBuffer( &ibv );

					D3D12_VERTEX_BUFFER_VIEW vbvs[2];
					vbvs[0] = g_resources.vertex_buffer_view( );
					vbvs[1].BufferLocation = m_sun_shadow_mesh_transforms_buffer->GetGPUVirtualAddress( );
					vbvs[1].SizeInBytes = max_mesh_list_size * sizeof(math::float4x4);
					vbvs[1].StrideInBytes = sizeof(math::float4x4);
					list->IASetVertexBuffers( 0, (u32)array_size( vbvs ), vbvs );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_0.y );
					list->ExecuteIndirect(
						m_draw_indexed_cs,
						max_mesh_list_size,
						m_sun_shadow_mesh_indirect_buffer, 0,
						m_constant_buffers[j], instance_count_offset
					);
				}

				{
					barrier_builder bb( list );
					bb.transition(
						m_image_vbuf_polygon_id,
						D3D12_RESOURCE_STATE_RENDER_TARGET,
						D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
					);
					bb.transition(
						g_resources.index_buffer( ),
						D3D12_RESOURCE_STATE_INDEX_BUFFER,
						D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
					);
					bb.transition(
						g_resources.vertex_buffer( ),
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
						D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
					);
					bb.transition(
						g_dx.swap_chain_buffer( i ),
						D3D12_RESOURCE_STATE_COMMON,
						D3D12_RESOURCE_STATE_RENDER_TARGET
					);
					bb.transition(
						m_image_sun_shadowmap,
						D3D12_RESOURCE_STATE_DEPTH_WRITE,
						D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
					);
				}

				{
					PROFILE_EVENT( list, j, shade );

					list.bind_pipeline( m_ps_shade );

					list.bind_root_cbv( 0, m_constant_buffers[j] );
					list.bind_root_srv( 0, g_resources.mesh_object_buffer( ) );
					list.bind_root_srv( 1, g_resources.index_buffer( ) );
					list.bind_root_srv( 2, g_resources.vertex_buffer( ) );
					list.bind_root_srv( 3, g_resources.vertex_data_buffer( ) );
					list.bind_root_srv( 4, g_resources.transforms( ).buffer( j ) );
					list.bind_root_srv( 5, g_resources.point_lights( ).buffer( j ) );
					list.bind_root_srv( 6, m_point_light_list[j] );

					list.bind_table_srv( 0, m_image_vbuf_polygon_id, m_image_vbuf_polygon_id.srv_cook( ) );
					list.bind_table_srv( 1, m_image_sun_shadowmap, m_image_sun_shadowmap.srv_cook( ) );
					list.bind_table_uav( 0, m_image_radiance, m_image_radiance.uav_cook( ) );

					math::u16x2 const group_count = m_ps_shade.calculate_groups_count( g_parameters.screen_resolution );
					list->Dispatch( group_count.x, group_count.y, 1 );
				}

				{
					barrier_builder bb( list );
					bb.transition(
						m_image_radiance,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
					);
				}

				{
					PROFILE_EVENT( list, j, resolve );

					list.bind_pipeline( m_ps_resolve );
					list.set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					list.bind_table_srv( shader_type_pixel, 0, m_image_radiance, m_image_radiance.srv_cook( ) );

					list->OMSetRenderTargets( 1, &m_backbuffer_rts[i], TRUE, nullptr );

					list->DrawInstanced( 3, 1, 0, 0 );
				}

				{
					barrier_builder bb( list );
					bb.transition(
						m_image_radiance,
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS
					);
				}

				{
					PROFILE_EVENT( list, j, render_ui );

					list.bind_pipeline( m_ps_render_ui );
					list.set_viewport_and_scissors( math::u32x2( g_parameters.screen_resolution ) );

					list->OMSetRenderTargets( 1, &m_backbuffer_rts[i], FALSE, nullptr );

					D3D12_VERTEX_BUFFER_VIEW const& vbv = m_ui_processor.vertex_buffer_view( j );
					list->IASetVertexBuffers( 0, 1, &vbv );

					list->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

					u32 const instance_count_offset = offsetof( gpu::constant_buffer, indirect_params_0.w );
					list->ExecuteIndirect(
						m_draw_cs,
						ui_processor::max_batches,
						m_ui_processor.command_buffer( j ), 0,
						m_constant_buffers[j], instance_count_offset
					);
				}

				{
					barrier_builder bb( list );
					bb.transition(
						g_dx.swap_chain_buffer( i ),
						D3D12_RESOURCE_STATE_RENDER_TARGET,
						D3D12_RESOURCE_STATE_PRESENT
					);
				}
			}

#ifdef USE_RENDER_PROFILING
			g_statistics.resolve( list, j );
#endif // #ifdef USE_RENDER_PROFILING
		}
	}
}

void render::destroy( )
{
	m_ready = false;

	// Wait for GPU.
	render_wait( [this]( ) { return m_gpu_frame_index->GetCompletedValue( ) >= m_frame_index - 1; } );

	m_scene_mesh_indirect_buffer.destroy( );
	m_scene_mesh_transforms_buffer.destroy( );
	m_sun_shadow_mesh_indirect_buffer.destroy( );
	m_sun_shadow_mesh_transforms_buffer.destroy( );

	destroy_effects( );

	m_copy_staging.destroy( );
	m_copy_fence.destroy( );
	for ( u32 i = 0; i < max_frame_delay; ++i )
	{
		m_copy_cmd_allocators[i].destroy( );
		m_copy_cmd_lists[i].destroy( );
	}

	m_image_vbuf_polygon_id.destroy( );
	m_image_radiance.destroy( );
	m_image_depth_buffer.destroy( );
	m_image_sun_shadowmap.destroy( );

	for ( u32 i = 0; i < max_frame_delay; ++i )
		m_constant_buffers[i].destroy( );

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
		m_mesh_list[i].destroy( );
		m_point_light_list[i].destroy( );
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
	u32 const cmd_index = m_frame_index % max_frame_delay;
	ID3D12CommandList* const current_cmd_list = m_cmd_lists[swap_chain_buffer_index][cmd_index];

	m_gpu_frame_index->SetEventOnCompletion( m_frame_index, m_gpu_frame_event.get_handle( ) );

	DX12_CHECK_RESULT( g_dx.queue_graphics( )->Wait( m_copy_fence, m_frame_index ) );
	g_dx.queue_graphics( )->ExecuteCommandLists( 1, &current_cmd_list );
	DX12_CHECK_RESULT( g_dx.swap_chain( )->Present( 0, 0 ) );
	DX12_CHECK_RESULT( g_dx.queue_graphics( )->Signal( m_gpu_frame_index, m_frame_index ) );
}

void render::update( )
{
	if ( !m_need_to_record_render )
	{
		// Wait for the submition we are going to replace.
		render_wait( [this]( ) { return m_gpu_frame_index->GetCompletedValue( ) >= m_frame_index - max_frame_delay; } );
	}
	else
	{
		// Wait for everything.
		render_wait( [this]( ) { return m_gpu_frame_index->GetCompletedValue( ) >= m_frame_index - 1; } );

		reload_render( );
		render_wait( [this]( ) { return ready( ); } );

		// This was set, but might not be reset (if WaitForMultipleObjects wasn't called).
		m_rs_queue_event.reset( );
		m_need_to_record_render = false;
	}

	// Prepare frame on the CPU.
	m_scene = m_next_scene;
	g_parameters_manager.update( );

	prepare_frame( );

	push_cmd_lists( );

	++m_frame_index;
}

void render::prepare_frame( )
{
	u32 const cmd_index = m_frame_index % max_frame_delay;

	bool const scene_has_sun = m_scene->get_sun_radiance( ) != math::float3( 0.0f );

	// Initialize render camera.
	m_render_camera.set_view( g_parameters.camera.view );
	m_render_camera.set_perspective( g_parameters.camera.fov, (float)g_parameters.screen_resolution.x / (float)g_parameters.screen_resolution.y, 0.1f, 1000.0f );
	m_render_camera.update( );

	lib::buffer_array<gpu_upload_task> copy_tasks( m_copy_tasks, max_copy_task_count );

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

		lib::buffer_array<u32> cpu_mesh_object_list( m_cpu_mesh_lists[cmd_index], max_mesh_list_size );

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
		mesh_list_task.set_buffer_upload( m_mesh_list[cmd_index], 0 );
		mesh_list_task.set_source_data( cpu_mesh_object_list.data( ), mesh_list_size * sizeof(u32) );

		if ( scene_has_sun )
		{
			lib::buffer_array<u32> cpu_sun_shadowmap_mesh_object_list( m_cpu_sun_shadow_mesh_lists[cmd_index], max_mesh_list_size );

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
			sun_shadowmap_mesh_list_task.set_buffer_upload( m_sun_shadow_mesh_list[cmd_index], 0 );
			sun_shadowmap_mesh_list_task.set_source_data( cpu_sun_shadowmap_mesh_object_list.data( ), sun_shadowmap_mesh_list_size * sizeof(u32) );
		}

		auto const& static_point_light_container = m_scene->static_point_light_container( );

		lib::buffer_array<u32> cpu_point_light_object_list( m_cpu_point_light_lists[cmd_index], max_point_light_list_size );

		for ( u32 i = 0; i < static_point_light_container.size( ); ++i )
		{
			math::float4 const& sphere = math::float4( static_point_light_container[i].m_data.position, static_point_light_container[i].m_data.range );
			if ( frustum.test_sphere_outside( sphere ) )
				continue;

			cpu_point_light_object_list.push_back( static_point_light_container[i].m_object_handle );
		}

		point_light_list_size = (u32)cpu_point_light_object_list.size( );

		gpu_upload_task& point_light_list_task = copy_tasks.emplace_back( );
		point_light_list_task.set_buffer_upload( m_point_light_list[cmd_index], 0 );
		point_light_list_task.set_source_data( cpu_point_light_object_list.data( ), point_light_list_size * sizeof(u32) );
	}

	// Process UI.
	u32 const ui_dispatch_dimension = m_ui_processor.prepare_ui( copy_tasks );

	// Fill constant buffers.
	gpu::constant_buffer& cpu_constant_buffer = m_cpu_constant_buffers[cmd_index];

	m_render_camera.fill_constant_buffer( cpu_constant_buffer );
	cpu_constant_buffer.viewport_size = math::float4( g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, 1.0f / g_parameters.screen_resolution.x, 1.0f / g_parameters.screen_resolution.y );
	cpu_constant_buffer.viewport_size_uint = math::u32x4( g_parameters.screen_resolution.x, g_parameters.screen_resolution.y, 0, 0 );

	if ( m_scene )
	{
		cpu_constant_buffer.sun_direction.vx = m_scene->get_sun_direction( );
		cpu_constant_buffer.sun_radiance.vx = m_scene->get_sun_radiance( );
		cpu_constant_buffer.sun_radiance.w = ( m_scene->get_sun_radiance( ) == math::float3( 0.0f ) ) ? 0.0f : 1.0f;

		if ( scene_has_sun )
		{
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
	}

	cpu_constant_buffer.indirect_params_0 = math::u32x4( mesh_list_size, sun_shadowmap_mesh_list_size, point_light_list_size, ui_dispatch_dimension );
	cpu_constant_buffer.indirect_params_1 = math::u32x4( ( mesh_list_size + 255 ) / 256, 1, 1, 0 );
	cpu_constant_buffer.indirect_params_2 = math::u32x4( ( sun_shadowmap_mesh_list_size + 255 ) / 256, 1, 1, 0 );

	gpu_upload_task& constants_task = copy_tasks.emplace_back( );
	constants_task.set_buffer_upload( m_constant_buffers[cmd_index], 0 );
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

	push_frame_copy( copy_tasks );
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

void render::push_frame_copy( lib::buffer_array<gpu_upload_task> in_tasks )
{
	if ( in_tasks.size( ) == 0 )
		return;

	u32 const cmd_index = m_frame_index % max_frame_delay;

	dx_command_allocator cmd_allocator = m_copy_cmd_allocators[cmd_index];
	dx_command_list cmd_list = m_copy_cmd_lists[cmd_index];

	DX12_CHECK_RESULT( cmd_allocator->Reset( ) );
	DX12_CHECK_RESULT( cmd_list->Reset( cmd_allocator, nullptr ) );

	uptr staging_size = 0;

	for ( u32 i = 0; i < in_tasks.size( ); ++i )
	{
		bool const result = in_tasks[i].record( m_copy_staging_data, 1 * Mb, staging_size, cmd_list, m_copy_staging );
		ASSERT( result );
	}

	DX12_CHECK_RESULT( cmd_list->Close( ) );

	ASSERT( staging_size != 0 );

	g_dx.queue_copy( )->ExecuteCommandLists( 1, (ID3D12CommandList**)&cmd_list );
	DX12_CHECK_RESULT( g_dx.queue_copy( )->Signal( m_copy_fence, m_frame_index ) );
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
#ifndef GUARD_RENDER_RENDER_H_INCLUDED
#define GUARD_RENDER_RENDER_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <resource_system/api.h>
#include <system/interlocked.h>
#include <ui/font.h>
#include "dx.h"
#include "dx_command_allocator.h"
#include "dx_command_list.h"
#include "dx_command_signature.h"
#include "dx_fence.h"
#include "dx_resource.h"
#include "render_resources.h"
#include "gpu_structures.h"
#include "gpu_uploader.h"
#include "pipeline_state.h"
#include "ui_processor.h"
#include "camera.h"

namespace render {

class scene;

class render
{
public:
	enum {
		max_mesh_list_size = 4096,
		max_point_light_list_size = 512
	};

public:
	void create( );
	void destroy( );
	bool ready( );
	
	inline void set_scene( scene* const in_scene ) { m_next_scene = in_scene; }

	void update( );

	void ui_add_quad(
		math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
		math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_handle const in_texture );
	void ui_add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color );
	void ui_next_level( );

	void reload( );

	inline u64 frame_index( ) const { return m_frame_index; }

private:
	void on_resources_created( queried_resources& in_resources );
	void on_resources_destroyed( );

	void fill_effect_tasks( lib::buffer_array<task_info>& in_tasks );
	void destroy_effects( );
	void record_render( );

	void reload_render( );
	void on_effects_created( );

	template<typename Functor>
	void render_wait( Functor const& in_functor );
	void render_wait( u64 const in_frame_index );

	void push_cmd_lists( );

	void prepare_frame( );
	void process_statistics( );
	void push_frame_copy( lib::buffer_array<gpu_upload_task> in_tasks );

private:
	enum { max_copy_task_count = 8 };
	enum : u32 { sun_shadowmap_dimension = 4096 };

	enum image_srvs
	{
		image_srv_v_buffer_polygon_id = 0,
		image_srv_depth_buffer,
		image_srv_radiance,
		image_srv_sun_shadowmap,

		image_srv_count
	};

	enum image_rtvs
	{
		image_rtv_v_buffer_polygon_id = 0,

		image_rtv_output_0,
		image_rtv_output_1,

		image_rtv_count
	};

	enum image_uavs
	{
		image_uav_radiance = 0,

		image_uav_count
	};

	enum image_dsvs
	{
		image_dsv_screen = 0,
		image_dsv_screen_readonly,
		image_dsv_sun_shadowmap,

		image_dsv_count
	};

private:
	u32 m_cpu_mesh_lists[max_frame_delay][max_mesh_list_size];
	u32 m_cpu_sun_shadow_mesh_lists[max_frame_delay][max_mesh_list_size];
	u32 m_cpu_point_light_lists[max_frame_delay][max_point_light_list_size];
	gpu::constant_buffer m_cpu_constant_buffers[max_frame_delay];
	gpu_upload_data m_copy_tasks[max_copy_task_count];

	ui_processor m_ui_processor;

	compute_ps m_ps_gen_arg_mesh;
	compute_ps m_ps_gen_arg_sun_shadowmap;
	graphics_ps m_ps_render_shadowmap_directional;
	graphics_ps m_ps_render_shadowmap;
	graphics_ps m_ps_render_mesh;
	compute_ps m_ps_shade;
	graphics_ps m_ps_resolve;
	graphics_ps m_ps_render_ui;

	dx_resource m_scene_mesh_indirect_buffer;
	dx_resource m_sun_shadow_mesh_indirect_buffer;
	dx_resource m_scene_mesh_transforms_buffer;
	dx_resource m_sun_shadow_mesh_transforms_buffer;
	dx_resource m_mesh_list[max_frame_delay];
	dx_resource m_sun_shadow_mesh_list[max_frame_delay];
	dx_resource m_point_light_list[max_frame_delay];

	render_target_2d m_image_vbuf_polygon_id;
	compute_target_2d m_image_radiance;
	depth_stencil_2d m_image_depth_buffer;
	depth_stencil_2d m_image_sun_shadowmap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_backbuffer_rts[max_frame_delay];

	dx_resource m_constant_buffers[max_frame_delay];

	camera m_render_camera;

	u64 m_frame_index;
	dx_fence m_gpu_frame_index;

	sys::system_event m_gpu_frame_event;
	sys::system_event m_rs_queue_event;
	sys::system_event m_gpu_uploader_event;

	dx_command_allocator m_cmd_allocator;
	dx_command_list m_cmd_lists[max_frame_delay][max_frame_delay];

	dx_command_signature m_dispatch_cs;
	dx_command_signature m_draw_cs;
	dx_command_signature m_draw_indexed_cs;

	dx_command_allocator m_copy_cmd_allocators[max_frame_delay];
	dx_command_list m_copy_cmd_lists[max_frame_delay];
	dx_resource m_copy_staging;
	pbyte m_copy_staging_data;
	dx_fence m_copy_fence;

	scene*	m_scene;
	scene*	m_next_scene;

	ui::font::ptr m_debug_font;

	bool volatile m_ready;
	bool volatile m_need_to_record_render;

};

extern render g_render;

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_H_INCLUDED
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
	enum { max_mesh_object_list_size = 4096 };

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

	inline u64 frame_index( ) const { return m_frame_index; }

private:
	void on_resources_created( queried_resources& in_resources );
	void on_resources_destroyed( );

	void prepare_frame( );
	void prepare_frame_copy( );

	void process_statistics( );

private:
	enum { max_copy_task_count = 5 };

private:
	ui_processor m_ui_processor;

	pipeline_state m_ps_generate_mesh_arguments;
	pipeline_state m_ps_render_mesh_objects;
	pipeline_state m_ps_shade;
	pipeline_state m_ps_render_ui;
	dx_resource m_indirect_arguments_buffer;
	dx_resource m_instance_transforms_buffer;
	dx_resource m_mesh_object_list[max_frame_delay];

	u32 m_cpu_mesh_object_lists[max_frame_delay][max_mesh_object_list_size];
	gpu::constant_buffer m_cpu_constant_buffers[max_frame_delay];
	gpu_upload_task m_copy_tasks[max_copy_task_count];

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

	scene*	m_scene;
	scene*	m_next_scene;

	ui::font::ptr m_debug_font;

	u32 m_effect_count;

	bool volatile m_ready;
	bool volatile m_copy_finished;

};

extern render g_render;

} // namespace render

#endif // #ifndef GUARD_RENDER_RENDER_H_INCLUDED
#ifndef GUARD_RENDER_UI_PROCESSOR_H_INCLUDED
#define GUARD_RENDER_UI_PROCESSOR_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <math/vector.h>

#include "dx.h"
#include "gpu_structures.h"
#include "gpu_uploader.h"

#include "handles.h"

namespace render {

class ui_processor
{
private:
	struct quad_data
	{
		math::u16x4 corners_position;
		math::u16x4 corners_texcoord;
		math::half4 mult_color;
		math::half4 add_color;
		texture_handle::value_type texture;
	};

	struct vertex_data
	{
		math::s16x2 position;
		math::u16x2 texcoord;
		math::half4 mult_color;
		math::half4 add_color;
		u32 texture_id;
	};

public:
	enum {
		max_quads = 8192,
		max_batches = 256
	};

public:
	void create( );
	void destroy( );

	void add_quad(
		math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
		math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_handle const in_texture );
	void add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color );
	void next_level( );

	u32 upload_task_count_needed( ) const;
	u32 prepare_ui( gpu_upload_task* const in_tasks );

	inline dx_resource const& command_buffer( u32 const in_index ) { ASSERT_CMP( in_index, <, max_frame_delay ); return m_ui_command_buffer[in_index]; }
	inline dx_resource const& vertex_buffer( u32 const in_index ) { ASSERT_CMP( in_index, <, max_frame_delay ); return m_ui_vertex_buffer[in_index]; }
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view( u32 const in_index ) const;

private:
	u32 m_batch_data[max_batches];
	quad_data m_quad_data[max_quads];
	vertex_data m_cpu_vertex_buffer[max_quads * 6];
	gpu::draw_indirect_command m_cpu_command_buffer[max_batches];
	u32 m_current_batch_count;
	u32 m_current_quad_count;
	u32 m_batched_quad_count;

	dx_resource m_ui_command_buffer[max_frame_delay];
	dx_resource m_ui_vertex_buffer[max_frame_delay];

};

} // namespace render

#endif // #ifndef GUARD_RENDER_UI_PROCESSOR_H_INCLUDED
#ifndef __render_ui_batch_h_included_
#define __render_ui_batch_h_included_

#include <types.h>
#include <math/vector.h>

#include <lib/dynamic_array.h>

#include "resource_ids.h"

namespace render {

class ui_batch
{
private:
	struct ui_quad_data
	{
		math::u16x4 corners_position;
		math::u16x4 corners_texcoord;
		math::half4 mult_color;
		math::half4 add_color;
		texture_id texture;
	};

	struct gpu_vertex_data
	{
		math::s16x2 position;
		math::u16x2 texcoord;
		math::half4 mult_color;
		math::half4 add_color;
	};

public:
	struct batch_data
	{
		u32 vertex_start_index;
		u32 vertex_count;
		texture_id texture;
	};

	enum {
		max_vertices = 64 * Kb,
		max_batches = 4096
	};

public:
	void create( );
	void destroy( );

	void add_quad(
		math::u16x4 const in_corners_position, math::u16x4 const in_corners_texcoord,
		math::half4 const& in_mult_color, math::half4 const& in_add_color, texture_id const in_texture );

	void add_color_quad( math::u16x4 const in_corners_position, math::half4 const& in_color );

	void next_level( );
	
	inline pcvoid get_buffer( ) const { return m_buffer.data( ); }
	inline u32 get_buffer_size( ) const { return sizeof(gpu_vertex_data) * (u32)m_buffer.size( ); }
	inline lib::dynamic_array<batch_data> const& get_batch_data( ) const { return m_batch_data; }

	void clear( );

private:
	lib::dynamic_array<ui_quad_data> m_temporal_memory;
	lib::dynamic_array<gpu_vertex_data> m_buffer;
	lib::dynamic_array<batch_data> m_batch_data;

};

} // namespace render

#endif // #ifndef __render_ui_batch_h_included_
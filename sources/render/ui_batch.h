#ifndef __render_ui_batch_h_included_
#define __render_ui_batch_h_included_

#include <types.h>
#include <math/vector.h>

#include <lib/extensible_array.h>

#include "texture.h"

namespace render {

class ui_batch
{
private:
	struct ui_quad_data
	{
		math::u16x4 corners_position;
		texture* texture;
		math::u16x4 corners_texcoord;
		math::half4 color;
	};

	struct gpu_vertex_data
	{
		math::s16x2 position;
		math::u16x2 texcoord;
		math::half4 color;
	};

public:
	struct batch_data
	{
		texture* texture;
		u32 vertex_start_index;
		u32 vertex_count;
	};

	enum { max_vertices = 64 * Kb };

public:
	void create( );
	void destroy( );

	void add_quad(
		math::u16x4 const in_corners_position, texture* const in_texture,
		math::u16x4 const in_corners_texcoord, math::half4 const& in_color );

	void next_level( );
	
	inline pcvoid get_buffer( ) const { return m_buffer.begin( ); }
	inline u32 get_buffer_size( ) const { return sizeof(gpu_vertex_data) * (u32)m_buffer.size( ); }
	inline extensible_array<batch_data> const& get_batch_data( ) const { return m_batch_data; }

	void clear( );

	void add_debug_quads( texture* in_texture );

private:
	extensible_array<ui_quad_data> m_temporal_memory;
	extensible_array<gpu_vertex_data> m_buffer;
	extensible_array<batch_data> m_batch_data;

};

} // namespace render

#endif // #ifndef __render_ui_batch_h_included_
#ifndef GUARD_RENDER_INPUT_LAYOUT_H_INCLUDED
#define GUARD_RENDER_INPUT_LAYOUT_H_INCLUDED

#include <types.h>

struct D3D11_INPUT_ELEMENT_DESC;

namespace render {

enum vertex_type : u8
{
	vertex_type_position_2d = 0,
	vertex_type_position,
	vertex_type_mesh,
	vertex_type_ui,

	vertex_type_count
};

D3D11_INPUT_ELEMENT_DESC* get_vertex_type_desc( vertex_type in_vertex_type );

u32	get_vertex_type_desc_size( vertex_type in_vertex_type );

u32	get_vertex_type_buffers_count( vertex_type in_vertex_type );

u32	get_vertex_type_size( u32 in_buffer_index, vertex_type in_vertex_type );

} // namespace render

#endif // #ifndef GUARD_RENDER_INPUT_LAYOUT_H_INCLUDED
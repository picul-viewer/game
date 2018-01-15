#ifndef __render_input_layout_h_included_
#define __render_input_layout_h_included_

#include <core/types.h>
#include <core/math.h>
#include "dx_include.h"

namespace render {

enum vertex_type
{
	vertex_type_default = 0,
	vertex_type_static_mesh,

	vertex_type_count
};

D3D11_INPUT_ELEMENT_DESC* get_vertex_type_desc( vertex_type in_vertex_type );

u32	get_vertex_type_desc_size( vertex_type in_vertex_type );

u32	get_vertex_type_buffers_count( vertex_type in_vertex_type );

u32	get_vertex_type_size( u32 in_buffer_index, vertex_type in_vertex_type );

} // namespace render

#endif // #ifndef __render_input_layout_h_included_
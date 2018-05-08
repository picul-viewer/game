#include "input_layout.h"
#include "mesh.h"

#include <macros.h>

namespace render {

D3D11_INPUT_ELEMENT_DESC vertex_type_position_2d_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC vertex_type_position_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC vertex_type_mesh_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R8G8B8A8_SNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC* vertex_type_descs[] = {
	vertex_type_position_2d_desc,
	vertex_type_position_desc,
	vertex_type_mesh_desc
};

u32 vertex_type_desc_sizes[] = {
	1,
	1,
	3
};

u32 vertex_type_buffers_counts[] = {
	1,
	1,
	1
};

u32 vertex_type_sizes[][mesh::max_vertex_buffers_count] = {
	{ 8 },
	{ 12 },
	{ 20 }
};

D3D11_INPUT_ELEMENT_DESC* get_vertex_type_desc( vertex_type in_vertex_type )
{
	ASSERT( ( in_vertex_type >= 0 ) && ( in_vertex_type < vertex_type_count ) );
	return vertex_type_descs[in_vertex_type];
}

u32	get_vertex_type_desc_size( vertex_type in_vertex_type )
{
	ASSERT( ( in_vertex_type >= 0 ) && ( in_vertex_type < vertex_type_count ) );
	return vertex_type_desc_sizes[in_vertex_type];
}

u32	get_vertex_type_buffers_count( vertex_type in_vertex_type )
{
	ASSERT( ( in_vertex_type >= 0 ) && ( in_vertex_type < vertex_type_count ) );
	return vertex_type_buffers_counts[in_vertex_type];
}

u32	get_vertex_type_size( u32 in_buffer_index, vertex_type in_vertex_type )
{
	ASSERT( in_buffer_index < mesh::max_vertex_buffers_count );
	ASSERT( ( in_vertex_type >= 0 ) && ( in_vertex_type < vertex_type_count ) );
	return vertex_type_sizes[in_vertex_type][in_buffer_index];
}

} // namespace render
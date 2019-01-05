#include "input_layout.h"
#include "dx_include.h"
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
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC vertex_type_ui_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R16G16_SNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_UNORM, 0, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 1, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC* vertex_type_descs[] = {
	vertex_type_position_2d_desc,
	vertex_type_position_desc,
	vertex_type_mesh_desc,
	vertex_type_ui_desc
};

u32 vertex_type_desc_sizes[] = {
	1,
	1,
	3,
	4
};

u32 vertex_type_buffers_counts[] = {
	1,
	1,
	1,
	1
};

u32 vertex_type_sizes[][mesh::max_vertex_buffers_count] = {
	{ 8 },
	{ 12 },
	{ 32 },
	{ 24 }
};

D3D11_INPUT_ELEMENT_DESC* get_vertex_type_desc( vertex_type in_vertex_type )
{
	ASSERT_CMP( in_vertex_type, <, vertex_type_count );
	return vertex_type_descs[in_vertex_type];
}

u32	get_vertex_type_desc_size( vertex_type in_vertex_type )
{
	ASSERT_CMP( in_vertex_type, <, vertex_type_count );
	return vertex_type_desc_sizes[in_vertex_type];
}

u32	get_vertex_type_buffers_count( vertex_type in_vertex_type )
{
	ASSERT_CMP( in_vertex_type, <, vertex_type_count );
	return vertex_type_buffers_counts[in_vertex_type];
}

u32	get_vertex_type_size( u32 in_buffer_index, vertex_type in_vertex_type )
{
	ASSERT_CMP( in_buffer_index, <, mesh::max_vertex_buffers_count );
	ASSERT_CMP( in_vertex_type, <, vertex_type_count );
	return vertex_type_sizes[in_vertex_type][in_buffer_index];
}

} // namespace render
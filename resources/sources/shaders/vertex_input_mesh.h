#ifndef __shader_vertex_input_mesh_h_included_
#define __shader_vertex_input_mesh_h_included_

struct vertex_input
{
	float3 position : POSITION;
	float2 tex_coords : TEXCOORD;
	float3 normal : NORMAL;
};

#endif // #ifndef __shader_vertex_input_mesh_h_included_
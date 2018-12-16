#ifndef __shader_vertex_input_ui_h_included_
#define __shader_vertex_input_ui_h_included_

struct vertex_input
{
	float2 position : POSITION;
	float2 tex_coords : TEXCOORD;
	float4 mult_color : COLOR0;
	float4 add_color : COLOR1;
};

#endif // #ifndef __shader_vertex_input_ui_h_included_
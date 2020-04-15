#ifndef __shader_forward_default_h_included_
#define __shader_forward_default_h_included_

struct vertex_output
{
	float4 position		: SV_Position;
	float2 tex_coords	: TEXCOORD0;
};

#endif // #ifndef __shader_forward_default_h_included_
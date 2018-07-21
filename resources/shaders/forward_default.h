#ifndef __shader_forward_default_constants_h_included_
#define __shader_forward_default_constants_h_included_

cbuffer cb_per_instance : register( b1 )
{
	float4x4 c_WVP;
};

Texture2D	t_diffuse	: register( t0 );
Texture2D	t_specular	: register( t1 );

struct vertex_output
{
	float2 tex_coords : TEXCOORD;
};

#endif // #ifndef __shader_forward_default_constants_h_included_
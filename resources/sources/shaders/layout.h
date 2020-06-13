#ifndef GUARD_SHADER_LAYOUT_H_INCLUDED
#define GUARD_SHADER_LAYOUT_H_INCLUDED

#include "structures.h"

ConstantBuffer<constant_buffer> g_constant_buffer : register( b0 );

Texture2D<uint2>	g_polygon_id	: register( t0, space8 );
Texture2D<float>	g_depth			: register( t1, space8 );
Texture2D<float4>	g_radiance		: register( t2, space8 );

RWTexture2D<float4>	g_radiance_uav	: register( u0, space8 );

static const uint g_max_textures = 256;
Texture2D g_textures[g_max_textures] : register( t0, space9 );

#endif // #ifndef GUARD_SHADER_LAYOUT_H_INCLUDED
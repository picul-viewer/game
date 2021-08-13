#ifndef GUARD_SHADER_LAYOUT_H_INCLUDED
#define GUARD_SHADER_LAYOUT_H_INCLUDED

#include "structures.h"

ConstantBuffer<constant_buffer> g_constant_buffer : register( b0 );

Texture2D g_textures[] : register( t0, space2 );

#endif // #ifndef GUARD_SHADER_LAYOUT_H_INCLUDED
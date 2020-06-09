#ifndef GUARD_SHADER_COMMON_H_INCLUDED
#define GUARD_SHADER_COMMON_H_INCLUDED

#include "layout.h"

uint load_u16( StructuredBuffer<uint> buffer, uint index )
{
	uint real_index = index >> 1;
	uint data = buffer[real_index];
	return (index & 1) ? data >> 16 : data & 0x0000FFFF;
}

float3 screen_position_to_view( float2 screen_uv, float view_depth )
{
	return view_depth * float3( g_constant_buffer.deprojection_coefficients__fov_scale.zw * ( 0.5f - screen_uv.xy ), 1.0f );
}

float3 screen_position_to_world_ray( float2 screen_uv )
{
	return g_constant_buffer.world_camera_ray_top_left.xyz +
		screen_uv.x * g_constant_buffer.world_camera_screen_ray_x.xyz +
		screen_uv.y * g_constant_buffer.world_camera_screen_ray_y.xyz;
}

float get_depth( uint2 pixel_index )
{
	return g_depth.Load( uint3( pixel_index, 0 ) ).x;
}

float get_view_depth( uint2 pixel_index )
{
	float hardware_depth = get_depth( pixel_index );
	return g_constant_buffer.deprojection_coefficients__fov_scale.y / ( hardware_depth - g_constant_buffer.deprojection_coefficients__fov_scale.x );
}

vertex_data_unpacked unpack_vertex_data( vertex_data data )
{	
	vertex_data_unpacked result;
	result.texcoord = data.texcoord;
	result.normal = uint3( data.normal.x & 0xFFFF, data.normal.x >> 16, data.normal.y ) / 65535.0f * 2.0f - 1.0f;
	return result;
}

#endif // #ifndef GUARD_SHADER_COMMON_H_INCLUDED
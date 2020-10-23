#ifndef GUARD_SHADER_COMMON_H_INCLUDED
#define GUARD_SHADER_COMMON_H_INCLUDED

#include "layout.h"

static const float c_pi = 3.1415926535898f;
static const float c_inv_pi = 1.0f / c_pi;
static const float c_eps = 1e-7f;
static const float c_half_float_eps = 6.1e-5;

uint load_u16( StructuredBuffer<uint> buffer, uint index )
{
	uint real_index = index >> 1;
	uint data = buffer[real_index];
	return (index & 1) ? data >> 16 : data & 0x0000FFFF;
}

void load_u16x3( StructuredBuffer<uint> buffer, uint index, out uint3 value )
{
	uint real_index = index >> 1;
	uint2 data = uint2( buffer[real_index], buffer[real_index + 1] );
	uint2 vals = uint2( data.x >> 16, data.y & 0x0000FFFF );
	bool shift = index & 1;
	value.x = shift ? vals.x : (data.x & 0x0000FFFF);
	value.y = shift ? vals.y : vals.x;
	value.z = shift ? (data.y >> 16) : vals.y;
}

float3 compute_barycentrics( float3 ro, float3 rd, float3 a, float3 b, float3 c )
{
	const float3 dp0 = ro - a;
	const float3 dp1 = b - a;
	const float3 dp2 = a - c;

	const float3 dett = cross(dp1, dp2);
	const float3 detuv = cross(dp0, rd);

	float den = dot(dett, rd);

	float3 det = -1.0 / den;

	float3 bc = float3(0.0f, dot(detuv, dp2), dot(detuv, dp1)) * det;
	bc.x = 1.0 - bc.y - bc.z;

	return bc;
}

float2 octahedron_encode( float3 v )
{
	float2 p = v.xy / ( abs( v.x ) + abs( v.y ) + abs( v.z ) );
	return ( v.z <= 0.0f ) ? ( 1.0f - abs( p.yx ) ) * ( ( p.xy >= 0.0 ) ? 1.0f : -1.0f ) : p;
}

uint octahedron_encode_uint( float3 v )
{
	float2 enc = octahedron_encode( v );
	return f32tof16( enc.x ) | ( f32tof16( enc.y ) << 16 );
}

float3 octahedron_decode_unnorm( float2 e )
{
	float3 v = float3( e.xy, 1.0f - abs( e.x ) - abs( e.y ) );
	float t = max( -v.z, 0.0f );
	v.xy += ( v.xy >= 0.0f ) ? -t : t;
	return v;
}

float3 octahedron_decode_uint_unnorm( uint e )
{
	float2 dec = float2( f16tof32( e ), f16tof32( e >> 16 ) );
	return octahedron_decode_unnorm( dec );
}

float3 octahedron_decode( float2 e )
{
	return normalize( octahedron_decode_unnorm( e ) );
}

float3 octahedron_decode_uint( uint e )
{
	return normalize( octahedron_decode_uint_unnorm( e ) );
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

void screen_position_to_world_rays_with_derivatives( float2 screen_uv, float2 inverse_viewport_size, out float3 ray, out float3 ray_right, out float3 ray_bottom )
{
	ray = g_constant_buffer.world_camera_ray_top_left.xyz +
		screen_uv.x * g_constant_buffer.world_camera_screen_ray_x.xyz +
		screen_uv.y * g_constant_buffer.world_camera_screen_ray_y.xyz;

	ray_right = ray + g_constant_buffer.world_camera_screen_ray_x.xyz * inverse_viewport_size.x;
	ray_bottom = ray + g_constant_buffer.world_camera_screen_ray_y.xyz * inverse_viewport_size.y;
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

uint mesh_list_size( )
{
	return g_constant_buffer.indirect_params_0.x;
}

uint sun_shadowmap_mesh_list_size( )
{
	return g_constant_buffer.indirect_params_0.y;
}

uint point_light_list_size( )
{
	return g_constant_buffer.indirect_params_0.z;
}

uint ui_batch_list_size( )
{
	return g_constant_buffer.indirect_params_0.w;
}

#endif // #ifndef GUARD_SHADER_COMMON_H_INCLUDED
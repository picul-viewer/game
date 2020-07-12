#ifndef GUARD_SHADER_SHADE_H_INCLUDED
#define GUARD_SHADER_SHADE_H_INCLUDED

#include "common.h"
#include "layout.h"
#include "structures.h"
#include "brdf.h"

float shadow_sun( float3 world_position, float3 geometric_normal )
{
	const float3 shadow_space_position = mul( float4( world_position, 1.0f ), transpose( g_constant_buffer.sun_view_projection ) ).xyz;
	const float2 shadow_coord = ( shadow_space_position.xy * float2( 0.5f, -0.5f ) + 0.5f ) * SUN_SHADOWMAP_DIMENSION;

	const uint2 sample_coord = clamp( uint2( shadow_coord ), 0, SUN_SHADOWMAP_DIMENSION - 1 );
	const float shadow_depth = g_sun_shadowmap.Load( uint3( sample_coord, 0 ) ) + c_half_float_eps;
	const float3 sampled_position = float3( ( sample_coord + 0.5f ) / SUN_SHADOWMAP_DIMENSION * float2( 2.0f, -2.0f ) - float2( 1.0f, -1.0f ), shadow_depth );

	const float3 transformed_normal = float3(
		geometric_normal.x * g_constant_buffer.sun_view_projection_inv_transp_0.w,
		geometric_normal.z * g_constant_buffer.sun_view_projection_inv_transp_1.w,
		dot( geometric_normal, g_constant_buffer.sun_view_projection_inv_transp_0.xyz )
	) / dot( geometric_normal, g_constant_buffer.sun_view_projection_inv_transp_1.xyz );
	
	const bool shadow_comparison = shadow_space_position.z <= shadow_depth;
	const bool shadow_normalized_comparison = ( transformed_normal.z < -0.01f ) && ( dot( transformed_normal.xyz, shadow_space_position - sampled_position ) > 0.0f );

	const float shadow_factor = ( shadow_comparison || shadow_normalized_comparison ) ? 1.0f : 0.05f;
	return shadow_factor;
}

float3 shade_sun( float3 albedo, float2 metalness_roughness, float3 v, float3 n, float3 geom_n, float3 i )
{
	if ( g_constant_buffer.sun_radiance.w == 0.0f )
		return 0.0f;
	
	const float3 o = g_constant_buffer.sun_direction.xyz;
	const float3 h = normalize( i + o );

	const float n_dot_h = dot( n, h );
	const float io_dot_h = dot( i, h );
	const float i_dot_n = dot( i, n );
	const float o_dot_n = dot( o, n );

	if ( o_dot_n <= 0.0f )
		return 0.0f;

	const float3 brdf_value = brdf( albedo, lerp( 0.05f, albedo, metalness_roughness.x ), metalness_roughness.y, n_dot_h, io_dot_h, i_dot_n, o_dot_n );
	const float shadow = shadow_sun( v, geom_n );

	return g_constant_buffer.sun_radiance.xyz * shadow * brdf_value * o_dot_n;
}

float3 shade_point( float3 albedo, float2 metalness_roughness, float3 n, float3 i, float3 position, point_light_object light )
{
	const float3 light_ray = light.position - position;
	const float squared_light_distance = dot( light_ray, light_ray );

	if ( squared_light_distance > light.range * light.range )
		return 0.0f;

	const float inv_light_distance = 1.0f / ( 1.0f + sqrt( squared_light_distance ) );
	const float3 o = light_ray * inv_light_distance;
	const float3 h = normalize( i + o );

	const float n_dot_h = dot( n, h );
	const float io_dot_h = dot( i, h );
	const float i_dot_n = dot( i, n );
	const float o_dot_n = dot( o, n );

	if ( o_dot_n <= 0.0f )
		return 0.0f;

	const float3 brdf_value = brdf( albedo, lerp( 0.05f, albedo, metalness_roughness.x ), metalness_roughness.y, n_dot_h, io_dot_h, i_dot_n, o_dot_n );

	return light.color.xyz * brdf_value * o_dot_n / squared_light_distance;
}

#endif // #ifndef GUARD_SHADER_SHADE_H_INCLUDED
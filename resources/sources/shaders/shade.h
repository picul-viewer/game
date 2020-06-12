#ifndef GUARD_SHADER_SHADE_H_INCLUDED
#define GUARD_SHADER_SHADE_H_INCLUDED

#include "common.h"
#include "structures.h"
#include "brdf.h"

float3 shade_sun( float3 albedo, float2 metalness_roughness, float3 n, float3 i )
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

	return g_constant_buffer.sun_radiance.xyz * brdf_value * o_dot_n;
}

float3 shade_point( float3 albedo, float2 metalness_roughness, float3 n, float3 i, float3 position, point_light_object light )
{
	const float3 light_ray = light.position - position;
	const float squared_light_distance = dot( light_ray, light_ray );

	if ( squared_light_distance > light.range * light.range )
		return 0.0f;

	const float inv_light_distance = 1.0f / sqrt( squared_light_distance );
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
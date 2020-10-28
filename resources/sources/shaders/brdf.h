#ifndef GUARD_SHADER_BRDF_H_INCLUDED
#define GUARD_SHADER_BRDF_H_INCLUDED

#include "common.h"

float3 f_schlick( float3 F0, float io_dot_h )
{
	const float m = 1.0f - io_dot_h;
	const float m2 = m * m;
	const float k = m2 * m2 * m;
	return F0 + ( 1.0f - F0 ) * k;
}

float d_ggx( float roughness, float n_dot_h )
{
	const float alpha = roughness * roughness;
	const float alpha2 = alpha * alpha;
	const float denom = n_dot_h * n_dot_h * ( alpha2 - 1.0f ) + 1.0f;
	return c_inv_pi * alpha2 / max( denom * denom, c_eps );
}

float g1_smith( float k, float r_dot_n )
{
	return r_dot_n / ( k + r_dot_n * ( 1.0f - k ) );
}

float g_smith( float roughness, float i_dot_n, float o_dot_n )
{
	const float k = 0.5f * roughness * roughness;
	return g1_smith( k, i_dot_n ) * g1_smith( k, o_dot_n );
}

float g_smith_disney( float roughness, float i_dot_n, float o_dot_n )
{
	return g_smith( ( roughness + 1.0f ) * 0.5f, i_dot_n, o_dot_n );
}

float3 lambertian_diffuse( float3 albedo )
{
	return c_inv_pi * albedo;
}

float3 brdf(
	float3 diffuse_albedo,
	float3 specular_albedo,
	float roughness,
	float n_dot_h,
	float io_dot_h,
	float i_dot_n,
	float o_dot_n
)
{
	const float clamped_i_dot_n = max( i_dot_n, c_eps );
	const float clamped_o_dot_n = max( o_dot_n, c_eps );
	
	const float3 diff = lambertian_diffuse( diffuse_albedo );
	
	/*const float d = d_ggx( roughness, n_dot_h );
	const float3 f = f_schlick( specular_albedo, io_dot_h );
	const float g = g_smith_disney( roughness, clamped_i_dot_n, clamped_o_dot_n );
	const float3 spec = d * f * g / ( 4.0f * clamped_i_dot_n * clamped_o_dot_n );*/

	// Optimized version.
	const float alpha = roughness * roughness;
	const float alpha2 = alpha * alpha;
	const float ggx_denom = n_dot_h * n_dot_h * ( alpha2 - 1.0f ) + 1.0f;
	const float disney_rough = ( roughness + 1.0f ) * 0.5f;
	const float k = 0.5f * disney_rough * disney_rough;
	const float3 f = f_schlick( specular_albedo, io_dot_h );
	const float numerator = 0.25f * c_inv_pi * alpha2;
	const float denominator = max( ggx_denom * ggx_denom, c_eps ) * ( k + clamped_i_dot_n * ( 1.0f - k ) ) * ( k + clamped_o_dot_n * ( 1.0f - k ) );
	const float3 spec = f * numerator / denominator;
	
	return diff + spec;
}

#endif // #ifndef GUARD_SHADER_BRDF_H_INCLUDED
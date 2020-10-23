#include "structures.h"
#include "layout.h"
#include "common.h"
#include "shade.h"

StructuredBuffer<mesh_object>			g_mesh_objects			: register( t0 );
StructuredBuffer<uint>					g_indices				: register( t1 );
StructuredBuffer<float3>				g_vertices				: register( t2 );
StructuredBuffer<vertex_data>			g_vertex_data			: register( t3 );
StructuredBuffer<float4x3>				g_transforms			: register( t4 );
StructuredBuffer<point_light_object>	g_point_light_objects	: register( t5 );
StructuredBuffer<uint>					g_point_light_list		: register( t6 );
SamplerState							g_sampler				: register( s0 );

vertex_data_unpacked interpolate_vertex_data( vertex_data_unpacked v0, vertex_data_unpacked v1, vertex_data_unpacked v2, float3 barycentrics )
{
	vertex_data_unpacked data;

	data.texcoord = barycentrics.x * v0.texcoord + barycentrics.y * v1.texcoord + barycentrics.z * v2.texcoord;
	data.normal = barycentrics.x * v0.normal + barycentrics.y * v1.normal + barycentrics.z * v2.normal;

	return data;
}

[numthreads( 16, 16, 1 )]
void main( uint3 id : SV_DispatchThreadID )
{
	if ( any( id.xy >= uint2( g_constant_buffer.viewport_size.xy ) ) )
		return;

	const uint2 polygon_id = g_polygon_id.Load( uint3( id.xy, 0 ) );

	if (polygon_id.x == 0)
	{
		g_radiance_uav[id.xy] = 0.0f;
		return;
	}

	const mesh_object mesh = g_mesh_objects[polygon_id.x - 1];
	const float4x3 transform = g_transforms[mesh.transform_index];

	uint3 ind;
	load_u16x3( g_indices, mesh.index_buffer_offset + 3 * polygon_id.y, ind );

	const float3 v0_local = g_vertices[mesh.vertex_buffer_offset + ind.x];
	const float3 v1_local = g_vertices[mesh.vertex_buffer_offset + ind.y];
	const float3 v2_local = g_vertices[mesh.vertex_buffer_offset + ind.z];

	float3 v0 = mul( transpose( transform ), float4( v0_local, 1.0f ) );
	float3 v1 = mul( transpose( transform ), float4( v1_local, 1.0f ) );
	float3 v2 = mul( transpose( transform ), float4( v2_local, 1.0f ) );

	float2 screen_uv = ( id.xy + 0.5f ) * g_constant_buffer.viewport_size.zw;
	float3 world_camera_ray, world_camera_ray_right, world_camera_ray_bottom;
	screen_position_to_world_rays_with_derivatives( screen_uv, g_constant_buffer.viewport_size.zw,
		world_camera_ray, world_camera_ray_right, world_camera_ray_bottom );

	const vertex_data_unpacked v0_info = unpack_vertex_data( g_vertex_data[mesh.vertex_buffer_offset + ind.x] );
	const vertex_data_unpacked v1_info = unpack_vertex_data( g_vertex_data[mesh.vertex_buffer_offset + ind.y] );
	const vertex_data_unpacked v2_info = unpack_vertex_data( g_vertex_data[mesh.vertex_buffer_offset + ind.z] );

	const float3 barycentrics = compute_barycentrics( g_constant_buffer.world_camera_position.xyz, world_camera_ray.xyz, v0.xyz, v1.xyz, v2.xyz );
	const float3 v = barycentrics.x * v0 + barycentrics.y * v1 + barycentrics.z * v2;
	const vertex_data_unpacked data = interpolate_vertex_data( v0_info, v1_info, v2_info, barycentrics );

	const vertex_data_unpacked data_right = interpolate_vertex_data( v0_info, v1_info, v2_info, compute_barycentrics( g_constant_buffer.world_camera_position.xyz, world_camera_ray_right.xyz, v0.xyz, v1.xyz, v2.xyz ) );
	const vertex_data_unpacked data_bottom = interpolate_vertex_data( v0_info, v1_info, v2_info, compute_barycentrics( g_constant_buffer.world_camera_position.xyz, world_camera_ray_bottom.xyz, v0.xyz, v1.xyz, v2.xyz ) );

	const float2 texcoord_ddx = data_right.texcoord.xy - data.texcoord.xy;
	const float2 texcoord_ddy = data_bottom.texcoord.xy - data.texcoord.xy;

	const float3 albedo = g_textures[NonUniformResourceIndex(mesh.albedo_texture_index)].SampleGrad( g_sampler, data.texcoord, texcoord_ddx, texcoord_ddy ).xyz;
	const float2 metalness_roughness = g_textures[NonUniformResourceIndex(mesh.metal_rough_texture_index)].SampleGrad( g_sampler, data.texcoord, texcoord_ddx, texcoord_ddy ).zy;

	const float3 n = normalize( mul( transpose( transform ), float4( data.normal.xyz, 0.0f ) ).xyz );
	const float3 geom_n = cross( v1 - v0, v2 - v0 );
	const float3 i = normalize( -world_camera_ray );

	float3 result = shade_sun( albedo, metalness_roughness, v, n, geom_n, i );

	for ( uint li = 0; li < point_light_list_size( ); ++li )
	{
		uint light_index = g_point_light_list[li];
		result += shade_point( albedo, metalness_roughness, n, i, v, g_point_light_objects[light_index] );
	}

	g_radiance_uav[id.xy] = float4( result, 0.0f );
}
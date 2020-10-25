#ifndef GUARD_SHADER_STRUCTURES_H_INCLUDED
#define GUARD_SHADER_STRUCTURES_H_INCLUDED

struct mesh_object
{
	uint index_buffer_offset;
	uint vertex_buffer_offset;
	uint index_count;
	uint vertex_count;
	uint transform_index;
	uint albedo_texture_index;
	uint metal_rough_texture_index;
	uint padding;
};

struct point_light_object
{
	float3 position;
	float range;
	float3 color;
	float squared_range;
};

struct vertex_data
{
	float2 texcoord;
	uint2 normal;
};

struct vertex_data_unpacked
{
	float2 texcoord;
	float3 normal;
};

struct constant_buffer
{
	float4x4 view_projection;
	float4x4 sun_view_projection;
	float4 sun_view_projection_inv_transp_0;
	float4 sun_view_projection_inv_transp_1;
	float4 world_camera_position;
	float4 world_camera_ray_top_left;
	float4 world_camera_screen_ray_x;
	float4 world_camera_screen_ray_y;
	float4 deprojection_coefficients__fov_scale;
	float4 viewport_size;
	float4 sun_direction;
	float4 sun_radiance;
	uint4 viewport_size_uint;

	// Mesh count, sun shadowmap mesh count, point light count, UI batch count
	uint4 indirect_params_0;
	// Mesh gen arg count, 1, 1, unused
	uint4 indirect_params_1;
	// Sun shadowmap mesh gen arg count, 1, 1, unused
	uint4 indirect_params_2;
};

struct dispatch_indirect_command
{
	uint group_count_x;
	uint group_count_y;
	uint group_count_z;
};

struct draw_indirect_command
{
	uint vertex_count_per_instance;
	uint instance_count;
	uint start_vertex_location;
	uint start_instance_location;
};

struct draw_indexed_indirect_command
{
	uint index_count_per_instance;
	uint instance_count;
	uint start_index_location;
	uint base_vertex_location;
	uint start_instance_location;
};

#endif // #ifndef GUARD_SHADER_STRUCTURES_H_INCLUDED
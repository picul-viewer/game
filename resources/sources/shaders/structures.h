#ifndef GUARD_SHADER_STRUCTURES_H_INCLUDED
#define GUARD_SHADER_STRUCTURES_H_INCLUDED

struct mesh_object
{
	uint index_buffer_offset;
	uint vertex_buffer_offset;
	uint index_count;
	uint vertex_count;
	uint transform_index;
	uint diffuse_texture_index;
	uint specular_texture_index;
	uint padding;
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
	float3x4 inv_view;
	float4 world_camera_position;
	float4 world_camera_ray_top_left;
	float4 world_camera_screen_ray_x;
	float4 world_camera_screen_ray_y;
	float4 deprojection_coefficients__fov_scale;
	float4 viewport_size;
	uint4 indirect_params_0;
	uint4 indirect_params_1;
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
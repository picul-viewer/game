#ifndef GUARD_RESOURCE_COMPILER_SCENE_COMPILE_UTILS_H_INCLUDED
#define GUARD_RESOURCE_COMPILER_SCENE_COMPILE_UTILS_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <math/vector.h>
#include <math/aabb.h>
#include <system/path.h>

namespace resource_compiler {

struct vertex_data_type
{
	math::float2 uv;
	math::unorm16x4 normal;
};

bool operator==( vertex_data_type const& l, vertex_data_type const& r );

class mesh_compiler
{
public:
	void compile( lib::buffer_array<math::aabb>& in_aabbs, pcstr const in_path );

	virtual bool generate_polygon( math::float3* in_vertex_positions, vertex_data_type* in_vertex_data ) = 0;

private:
	void save_mesh(
		pcstr const in_path,
		lib::buffer_array<math::float3> vertices,
		lib::buffer_array<vertex_data_type> vertices_data,
		lib::buffer_array<u16> indices
	);

};

void write_model_mesh_to_file(
	pcstr const in_albedo_name,
	pcstr const in_metal_rough_name,
	pcstr const in_mesh_path,
	pcstr const in_model_mesh_path,
	sys::path const& in_textures_path,
	math::aabb const& in_aabb
);

class scene_compiler
{
public:
	scene_compiler( pcstr const in_input_path, pcstr const in_output_path );
	~scene_compiler( );

	void add_mesh(
		mesh_compiler* const in_mesh_compiler,
		pcstr const in_albedo_name,
		pcstr const in_metal_rough_name,
		math::float4x3 const& in_transform
	);

	void compile( );

private:
	struct mesh_desc
	{
		sys::path model_mesh_path;
		math::float4x3 transform;
	};

private:
	lib::buffer_array<mesh_desc> m_meshes;
	lib::buffer_array<math::aabb> m_aabbs;
	sys::path m_textures_path;
	sys::path m_mesh_path;
	sys::path m_model_mesh_path;
	sys::path m_scene_path;
	pointer m_data;

};

} // namespace resource_compiler

#endif // #ifndef GUARD_RESOURCE_COMPILER_SCENE_COMPILE_UTILS_H_INCLUDED
#include "obj_compiler.h"

#include <macros.h>

#include <Shlwapi.h>

#include <math/aabb.h>
#include <math/bvh.h>
#include <math/math_3d.h>
#include <math/vector.h>

#include <lib/allocator.h>
#include <lib/buffer_array.h>

#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include <lib/linear_set.h>

#include <lib/writer.h>

#include <resources/resources_path.h>

#include <system/path.h>
#include <system/file.h>
#include <system/file_iterator.h>

#include "scene_compile_utils.h"

#define FAST_OBJ_IMPLEMENTATION
#include "external/fast_obj.h"

namespace resource_compiler {

void obj_compiler::create( )
{ }

void obj_compiler::destroy( )
{ }

class obj_mesh_compiler : public mesh_compiler
{
public:
	obj_mesh_compiler( pcstr const in_input_path ) :
		m_obj( fast_obj_read( in_input_path ) ),
		m_group_index( 0 ),
		m_face_index( 0 ),
		m_material_index( 0 )
	{
		if ( ( m_group_index >= m_obj->group_count ) || ( m_face_index >= m_obj->groups[m_group_index].face_count ) )
			return;

		u32 const face_offset = m_obj->groups[m_group_index].face_offset;

		m_material_index = m_obj->face_materials[face_offset + m_face_index];
	}

	~obj_mesh_compiler( )
	{
		fast_obj_destroy( m_obj );
	}

	bool generate_polygon( math::float3* in_vertex_positions, vertex_data_type* in_vertex_data ) override
	{
		u32 const face_count = m_obj->groups[m_group_index].face_count;

		if ( m_face_index >= face_count )
			return false;

		u32 const face_offset = m_obj->groups[m_group_index].face_offset;
		u32 const index_offset = m_obj->groups[m_group_index].index_offset;

		u32 currrent_material_index = m_obj->face_materials[face_offset + m_face_index];

		if ( m_material_index != currrent_material_index )
			return false;

		u32 const vertex_count = m_obj->face_vertices[face_offset + m_face_index];
		enum { vertices_per_face = 3 };
		ASSERT_CMP( vertex_count, ==, vertices_per_face );

		for ( u32 i = 0; i < vertices_per_face; ++i )
		{
			u32 const index_index = ( i == 2 ) ? 1 : ( i << 1 );
			auto const& obj_ind = m_obj->indices[index_offset + m_face_index * 3 + index_index];

			in_vertex_positions[i] = math::float3(
				m_obj->positions[3 * obj_ind.p + 0],
				m_obj->positions[3 * obj_ind.p + 1],
				-m_obj->positions[3 * obj_ind.p + 2]
			);

			in_vertex_data[i].normal.vx = math::normalize( math::float3(
				m_obj->normals[3 * obj_ind.n + 0],
				m_obj->normals[3 * obj_ind.n + 1],
				-m_obj->normals[3 * obj_ind.n + 2]
			) ) * 0.5f + 0.5f;
			in_vertex_data[i].normal.w = 0.0f;
			in_vertex_data[i].uv = math::float2(
				m_obj->texcoords[2 * obj_ind.t + 0],
				1.0f - m_obj->texcoords[2 * obj_ind.t + 1]
			);
		}

		++m_face_index;

		return true;
	}

	bool finished( ) const
	{
		return m_group_index >= m_obj->group_count;
	}

	void go_to_next( )
	{
		u32 const face_count = m_obj->groups[m_group_index].face_count;

		if ( m_face_index >= face_count )
		{
			m_face_index = 0;
			++m_group_index;
			return;
		}

		u32 const face_offset = m_obj->groups[m_group_index].face_offset;
		u32 const index_offset = m_obj->groups[m_group_index].index_offset;

		u32 currrent_material_index = m_obj->face_materials[face_offset + m_face_index];

		m_material_index = currrent_material_index;
	}

	pcstr current_diffuse( ) const
	{
		return m_obj->materials[m_material_index].map_Kd.name;
	}

	pcstr current_specular( ) const
	{
		return m_obj->materials[m_material_index].map_Ks.name;
	}

private:
	fastObjMesh* m_obj;
	u32 m_group_index;
	u32 m_face_index;
	u32 m_material_index;

};

void obj_compiler::compile( u64 relevant_date, weak_const_string input_file_name, weak_const_string output_directory )
{
	ASSERT( strings::equal( sys::path::get_file_extension( input_file_name ), ".obj" ), "wrong file extension: %s", sys::path::get_file_extension( input_file_name ) );

	little_string file_name = sys::path::get_file_name( input_file_name );
	sys::path::remove_file_extension( file_name.data( ) );

	sys::path scene_path = output_directory;
	scene_path += file_name + ".scene.cfg";

	sys::file_iterator fi;
	fi.create( scene_path.c_str( ) );
	if ( fi.is_valid( ) )
		if ( fi.get_modification_time( ) > relevant_date )
		{
			LOG( "obj_compiler: scene update is not required: \"%s\"\n", scene_path.c_str( ) );
			return;
		}

	scene_compiler compiler( input_file_name, output_directory );

	obj_mesh_compiler obj_compiler( input_file_name );

	while ( !obj_compiler.finished( ) )
	{
		pcstr const diffuse = obj_compiler.current_diffuse( );
		pcstr const specular = obj_compiler.current_specular( );
		compiler.add_mesh( &obj_compiler, diffuse, specular, math::float4x3::identity( ) );
		obj_compiler.go_to_next( );
	}

	compiler.compile( );

	LOG( "obj_compiler: obj scene compiled successfully: \"%s\"\n", scene_path.c_str( ) );
}

} // namespace resource_compiler
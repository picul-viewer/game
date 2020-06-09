#include "gltf_compiler.h"

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

#define CGLTF_IMPLEMENTATION
#include "external/cgltf.h"

namespace resource_compiler {

void gltf_compiler::create( )
{ }

void gltf_compiler::destroy( )
{ }

class gltf_mesh_compiler : public mesh_compiler
{
public:
	gltf_mesh_compiler( cgltf_primitive const& in_primitive ) :
		m_primitive( in_primitive ),
		m_data( in_primitive.attributes[0].data->count * ( sizeof(math::float3) + sizeof(math::float2) + sizeof(math::float3) ) ),
		m_index( 0 )
	{
		cgltf_size const num_vertices = in_primitive.attributes[0].data->count;

		m_positions.create( (pbyte)m_data, num_vertices, num_vertices );
		m_texcoords.create( (pbyte)m_data + num_vertices * sizeof(math::float3), num_vertices, num_vertices );
		m_normals.create( (pbyte)m_data + num_vertices * ( sizeof(math::float3) + sizeof(math::float2) ), num_vertices, num_vertices );

		for ( cgltf_size j = 0; j < in_primitive.attributes_count; ++j )
		{
			cgltf_attribute const& attribute = in_primitive.attributes[j];
			cgltf_accessor* const accessor = attribute.data;
			cgltf_size const accessor_count = accessor->count;

			ASSERT( num_vertices == accessor_count );

			cgltf_size const float_count = cgltf_accessor_unpack_floats( accessor, nullptr, 0 );

			if ( ( attribute.type == cgltf_attribute_type_position ) && ( attribute.index == 0 ) )
			{
				ASSERT( float_count == 3 * num_vertices );
				cgltf_accessor_unpack_floats( accessor, (float*)m_positions.data( ), float_count );
			}
			else if ( ( attribute.type == cgltf_attribute_type_texcoord ) && ( attribute.index == 0 ) )
			{
				ASSERT( float_count == 2 * num_vertices );
				cgltf_accessor_unpack_floats( accessor, (float*)m_texcoords.data( ), float_count );
			}
			else if ( ( attribute.type == cgltf_attribute_type_normal ) && ( attribute.index == 0 ) )
			{
				ASSERT( float_count == 3 * num_vertices );
				cgltf_accessor_unpack_floats( accessor, (float*)m_normals.data( ), float_count );
			}
		}
	}

	bool generate_polygon( math::float3* in_vertex_positions, vertex_data_type* in_vertex_data ) override
	{
		if ( m_primitive.indices != nullptr )
		{
			cgltf_accessor* const accessor = m_primitive.indices;

			if ( m_index >= accessor->count )
				return false;

			for( u32 i = 0; i < 3; ++i )
			{
				u32 const index_index = ( i == 2 ) ? 1 : ( i << 1 );
				u32 const vertex_index = (u32)cgltf_accessor_read_index(accessor, m_index + index_index );
				in_vertex_positions[i] = math::float3(
					m_positions[vertex_index].x,
					m_positions[vertex_index].y,
					-m_positions[vertex_index].z
				);
				in_vertex_data[i].uv = m_texcoords[vertex_index];
				in_vertex_data[i].normal.vx = math::normalize( math::float3(
					m_normals[vertex_index].x,
					m_normals[vertex_index].y,
					-m_normals[vertex_index].z
				) ) * 0.5f + 0.5f;
				in_vertex_data[i].normal.w = 0.0f;
			}
		}
		else
		{
			if ( m_index >= m_positions.size( ) )
				return false;

			for( u32 i = 0; i < 3; ++i )
			{
				u32 vertex_index = m_index + i;
				in_vertex_positions[i] = math::float3(
					m_positions[vertex_index].x,
					m_positions[vertex_index].y,
					-m_positions[vertex_index].z
				);
				in_vertex_data[i].uv = m_texcoords[vertex_index];
				in_vertex_data[i].normal.vx = math::normalize( math::float3(
					m_normals[vertex_index].x,
					m_normals[vertex_index].y,
					-m_normals[vertex_index].z
				) ) * 0.5f + 0.5f;
				in_vertex_data[i].normal.w = 0.0f;
			};
		}

		m_index += 3;
		return true;
	}

private:
	lib::buffer_array<math::float3> m_positions;
	lib::buffer_array<math::float2> m_texcoords;
	lib::buffer_array<math::float3> m_normals;
	cgltf_primitive const& m_primitive;
	std_allocator::scoped_memory m_data;
	u32 m_index;

};

void process_node(
	cgltf_node const& in_node,
	scene_compiler& compiler
)
{
	cgltf_mesh* const mesh = in_node.mesh;

	if ( mesh != nullptr )
	{
		math::float4x4 node_to_world;
		cgltf_node_transform_world( &in_node, node_to_world.data );

		for ( cgltf_size i = 0; i < mesh->primitives_count; ++i )
		{
			cgltf_primitive const& primitive = mesh->primitives[i];

			gltf_mesh_compiler mesh_compiler( primitive );

			pcstr diffuse_name = nullptr;
			pcstr specular_name = nullptr;

			if ( ( primitive.material != nullptr ) && primitive.material->has_pbr_metallic_roughness )
			{
				cgltf_texture* const diffuse_texture = primitive.material->pbr_metallic_roughness.base_color_texture.texture;
				if ( diffuse_texture ) diffuse_name = diffuse_texture->image->uri;

				cgltf_texture* const specular_texture = primitive.material->pbr_metallic_roughness.metallic_roughness_texture.texture;
				if ( specular_texture ) specular_name = specular_texture->image->uri;
			}

			compiler.add_mesh( &mesh_compiler, diffuse_name, specular_name, node_to_world );
		}
	}

	for ( cgltf_size i = 0; i < in_node.children_count; ++i )
	{
		cgltf_node* const node = in_node.children[i];
		ASSERT( node );
		process_node( *node, compiler );
	}
}

void gltf_compiler::compile( u64 relevant_date, weak_const_string input_file_name, weak_const_string output_directory )
{
	ASSERT( strings::equal( sys::path::get_file_extension( input_file_name ), ".gltf" ), "wrong file extension: %s", sys::path::get_file_extension( input_file_name ) );

	little_string file_name = sys::path::get_file_name( input_file_name );
	sys::path::remove_file_extension( file_name.data( ) );

	sys::path scene_path = output_directory;
	scene_path += file_name + ".scene.cfg";

	sys::file_iterator fi;
	fi.create( scene_path.c_str( ) );
	if ( fi.is_valid( ) )
		if ( fi.get_modification_time( ) > relevant_date )
		{
			LOG( "gltf_compiler: scene update is not required: \"%s\"\n", scene_path.c_str( ) );
			return;
		}

	cgltf_options options = { };
	cgltf_data* data = nullptr;
	cgltf_result result = cgltf_parse_file( &options, input_file_name, &data );

	if ( result == cgltf_result_success )
		result = cgltf_load_buffers( &options, data, input_file_name );

	if ( result != cgltf_result_success )
	{
		pcstr message = nullptr;

		switch ( result )
		{
			case cgltf_result_data_too_short:	message = "gltf_compiler: gltf importer error - data_too_short: \"%s\"\n";	break;
			case cgltf_result_unknown_format:	message = "gltf_compiler: gltf importer error - unknown_format: \"%s\"\n";	break;
			case cgltf_result_invalid_json:		message = "gltf_compiler: gltf importer error - invalid_json: \"%s\"\n";	break;
			case cgltf_result_invalid_gltf:		message = "gltf_compiler: gltf importer error - invalid_gltf: \"%s\"\n";	break;
			case cgltf_result_invalid_options:	message = "gltf_compiler: gltf importer error - invalid_options: \"%s\"\n";	break;
			case cgltf_result_file_not_found:	message = "gltf_compiler: gltf importer error - file_not_found: \"%s\"\n";	break;
			case cgltf_result_io_error:			message = "gltf_compiler: gltf importer error - io_error: \"%s\"\n";		break;
			case cgltf_result_out_of_memory:	message = "gltf_compiler: gltf importer error - out_of_memory: \"%s\"\n";	break;
			case cgltf_result_legacy_gltf:		message = "gltf_compiler: gltf importer error - legacy_gltf: \"%s\"\n";		break;
			default: UNREACHABLE_CODE
		}

		LOG( message, scene_path.c_str( ) );
		return;
	}

	ASSERT( data->scenes_count > 0 );
	cgltf_scene const& scene = data->scenes[0];

	scene_compiler compiler( input_file_name, output_directory );

	for ( cgltf_size i = 0; i < scene.nodes_count; ++i )
	{
		cgltf_node* node = scene.nodes[i];
		ASSERT( node );
		process_node( *node, compiler );
	}

	compiler.compile( );

	cgltf_free(data);

	LOG( "gltf_compiler: gltf scene compiled successfully: \"%s\"\n", scene_path.c_str( ) );
}

} // namespace resource_compiler
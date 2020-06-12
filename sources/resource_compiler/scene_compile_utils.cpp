#include "scene_compile_utils.h"

#include <macros.h>

#include <lib/allocator.h>

#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include <lib/linear_set.h>

#include <lib/writer.h>

#include <math/bvh.h>

#include <resources/resources_path.h>

#include <system/file.h>

namespace resource_compiler {

bool operator==( vertex_data_type const& l, vertex_data_type const& r )
{
	return
		( l.uv		== r.uv		) &&
		( l.normal	== r.normal	);
}

void mesh_compiler::compile( lib::buffer_array<math::aabb>& in_aabbs, pcstr const in_path )
{
	enum : u32 {
		max_indices = 3 * 1024 * 1024,
		max_vertices = 65536
	};

	std_allocator::scoped_memory const vertex_memory( max_vertices * ( sizeof(math::float3) + sizeof(vertex_data_type) ) );
	pbyte const vertices_memory = vertex_memory;
	pbyte const vertices_data_memory = vertices_memory + max_vertices * sizeof(math::float3);

	lib::buffer_array<math::float3> vertices;
	lib::buffer_array<vertex_data_type> vertices_data;
	vertices.create( vertices_memory, max_vertices );
	vertices_data.create( vertices_data_memory, max_vertices );

	lib::linear_set<u16, max_vertices * 2> vertices_map;
	vertices_map.create( );

	std_allocator::scoped_memory const indices_data( max_indices * sizeof(u16) );
	lib::buffer_array<u16> indices;
	indices.create( indices_data, max_indices );

	math::aabb aabb;
	aabb.set_min_max( math::float3( FLT_MAX ), math::float3( FLT_MIN ) );

	math::float3 vertex_positions[3];
	vertex_data_type vertex_data[3];

	while ( generate_polygon( vertex_positions, vertex_data ) )
	{
		u32 new_v_count = 0;

		for ( u32 i = 0; i < 3; ++i )
		{
			bool found = vertices_map.find_if(
				lib::hash( vertex_positions[i], lib::hash( vertex_data[i] ) ),
				[i, vertex_positions, vertex_data, &vertices, &vertices_data]( u32 const value )
				{
					return ( vertices[value] == vertex_positions[i] ) && ( vertices_data[value] == vertex_data[i] );
				}
			);

			new_v_count += found ? 0 : 1;
		}

		if ( vertices.size( ) + new_v_count > max_vertices )
		{
			save_mesh( format( in_path, (u32)in_aabbs.size( ) ).c_str( ), vertices, vertices_data, indices );
			in_aabbs.push_back( aabb );

			vertices.clear( );
			vertices_data.clear( );
			indices.clear( );
			vertices_map.clear( );
			aabb.set_min_max( math::float3( FLT_MAX ), math::float3( FLT_MIN ) );
		}

		for ( u32 i = 0; i < 3; ++i )
		{
			u16* vertex_index;
			bool const found = vertices_map.find_if_or_insert(
				lib::hash( vertex_positions[i], lib::hash( vertex_data[i] ) ),
				[i, vertex_positions, vertex_data, &vertices, &vertices_data]( u32 const value )
				{
					return ( vertices[value] == vertex_positions[i] ) && ( vertices_data[value] == vertex_data[i] );
				},
				[i, vertex_positions, vertex_data, &vertices, &vertices_data, &aabb]( )
				{
					aabb.extrude( vertex_positions[i] );
					vertices.emplace_back( ) = vertex_positions[i];
					vertices_data.emplace_back( ) = vertex_data[i];
					return (u32)( vertices.size( ) - 1 );
				},
				vertex_index
			);

			indices.push_back( *vertex_index );
		}
	}

	if ( indices.size( ) != 0 )
	{
		save_mesh( format( in_path, (u32)in_aabbs.size( ) ).c_str( ), vertices, vertices_data, indices );
		in_aabbs.push_back( aabb );
	}
}

void mesh_compiler::save_mesh(
	pcstr const in_path,
	lib::buffer_array<math::float3> vertices,
	lib::buffer_array<vertex_data_type> vertices_data,
	lib::buffer_array<u16> indices
)
{
	u32 const index_count = (u32)indices.size( );
	u32 const vertex_count = (u32)vertices.size( );

	uptr const total_size = sizeof(u32) * 2 + index_count * sizeof(u16) + vertex_count * ( sizeof(math::float3) + sizeof(vertex_data_type) );

	std_allocator::scoped_memory const file_data_ptr( total_size );
	pointer const file_data = file_data_ptr;

	lib::writer w( file_data, total_size );

	w.write( index_count );
	w.write( vertex_count );
	w.write_data( indices.data( ), index_count * sizeof(u16) );
	w.write_data( vertices.data( ), vertex_count * sizeof(math::float3) );
	w.write_data( vertices_data.data( ), vertex_count * sizeof(vertex_data_type) );

	sys::file f;
	f.create( in_path, sys::file::open_write );
	if ( !f.is_valid( ) )
	{
		LOG( "obj_compiler: unable to create a file: \"%s\"\n", in_path );
		return;
	}

	f.write( file_data, total_size );
	f.destroy( );
}

void write_model_mesh_to_file(
	pcstr const in_albedo_name,
	pcstr const in_metal_rough_name,
	pcstr const in_mesh_path,
	pcstr const in_model_mesh_path,
	sys::path const& in_textures_path,
	math::aabb const& in_aabb
)
{
	sys::path albedo = GET_RESOURCE_PATH( "textures/albedo.dds" );

	if ( in_albedo_name )
	{
		little_string albedo_name = in_albedo_name;
		sys::path::remove_file_extension( albedo_name );
		albedo_name += ".dds";
		albedo = in_textures_path + albedo_name;
	}

	sys::path metal_rough = GET_RESOURCE_PATH( "textures/metal_rough.dds" );

	if ( in_metal_rough_name )
	{
		little_string metal_rough_name = in_metal_rough_name;
		sys::path::remove_file_extension( metal_rough_name );
		metal_rough_name += ".dds";
		metal_rough = in_textures_path + metal_rough_name;
	}

	uptr const model_mesh_memory_size = 4 * Kb;
	std_allocator::scoped_memory const model_mesh_memory( model_mesh_memory_size );
	lib::writer model_mesh_output( model_mesh_memory, model_mesh_memory_size );

	model_mesh_output.write( in_aabb.get_min( ) );
	model_mesh_output.write( in_aabb.get_max( ) );
	model_mesh_output.write_str( in_mesh_path );
	model_mesh_output.write_str( albedo.c_str( ) );
	model_mesh_output.write_str( metal_rough.c_str( ) );

	sys::file f;
	f.create( in_model_mesh_path, sys::file::open_write );
	ASSERT( f.is_valid( ) );
	f.write( model_mesh_memory, model_mesh_output.size( model_mesh_memory ) );
	f.destroy( );
}

scene_compiler::scene_compiler( pcstr const in_input_path, pcstr const in_output_path )
{
	u32 const max_mesh_count = 16384;

	uptr const memory_size =
		max_mesh_count * sizeof(mesh_desc) +
		max_mesh_count * sizeof(math::aabb);

	m_data = std_allocator( ).allocate( memory_size );

	pointer p = m_data;

	m_meshes.create( p, max_mesh_count );
	p += max_mesh_count * sizeof(mesh_desc);
	m_aabbs.create( p, max_mesh_count );
	p += max_mesh_count * sizeof(math::aabb);

	ASSERT_CMP( p, ==, m_data + memory_size );

	little_string file_name = sys::path::get_file_name( in_input_path );
	sys::path::remove_file_extension( file_name.data( ) );

	m_scene_path = in_output_path;
	m_scene_path += file_name + ".scene.cfg";

	m_textures_path = sys::path::rebase( in_input_path, strings::length( GET_RESOURCE_SOURCE_PATH( "scenes" ) ), GET_RESOURCE_PATH( "textures/scenes" ) );
	m_textures_path.remove_file_extension( );

	m_mesh_path = sys::path( in_output_path ) + format( "%s_%%d.mesh", file_name.c_str( ) );
	m_model_mesh_path = sys::path( in_output_path ) + format( "%s_%%d.render_model_mesh.cfg", file_name.c_str( ) );
}

scene_compiler::~scene_compiler( )
{
	std_allocator( ).deallocate( m_data );
}

void scene_compiler::add_mesh(
	mesh_compiler* const in_mesh_compiler,
	pcstr const in_albedo_name,
	pcstr const in_metal_rough_name,
	math::float4x3 const& in_transform
)
{
	uptr const pre_count = m_aabbs.size( );

	in_mesh_compiler->compile( m_aabbs, m_mesh_path.c_str( ) );

	uptr const post_count = m_aabbs.size( );

	for ( uptr i = pre_count; i < post_count; ++i )
	{
		m_meshes.push_back( { format( m_model_mesh_path.c_str( ), (u32)i ), in_transform } );

		write_model_mesh_to_file(
			in_albedo_name, in_metal_rough_name, format( m_mesh_path.c_str( ), (u32)i ),
			m_meshes.back( ).model_mesh_path.c_str( ), m_textures_path, m_aabbs[i]
		);

		m_aabbs[i].modify( in_transform );
	}
}

void scene_compiler::compile( )
{
	uptr const output_size = 16 * Mb;
	std_allocator::scoped_memory output_data( output_size );
	lib::writer output;
	output.create( output_data, output_size );

	u32* const render_size = output.ptr( );
	output += sizeof(u32);
	pvoid const render_output = output.ptr( );

	u32 const mesh_count = (u32)m_meshes.size( );
	output.write( mesh_count );

	for ( mesh_desc* i = m_meshes.begin( ); i < m_meshes.end( ); ++i )
	{
		output.write_str( i->model_mesh_path.c_str( ) );
		output.write( i->transform );
	}

	uptr const bvh_memory_size = math::bvh::node_data_size * 2 * mesh_count;
	pointer bvh_memory = stack_allocate( bvh_memory_size );

	u32* const bvh_handles = stack_allocate( mesh_count * sizeof(u32) );
	lib::iota( bvh_handles, bvh_handles + mesh_count, 0 );

	math::bvh static_mesh_bvh;
	static_mesh_bvh.create( bvh_memory, bvh_memory_size );
	static_mesh_bvh.build( m_aabbs.data( ), bvh_handles, mesh_count );
	static_mesh_bvh.serialize( output );
	static_mesh_bvh.destroy( );

	*render_size = (u32)output.size( render_output );

	sys::file f;
	f.create( m_scene_path.c_str( ), sys::file::open_write );
	ASSERT( f.is_valid( ) );
	f.write( output_data, output.size( output_data ) );
	f.destroy( );
}

} // namespace resource_compiler
#include "fbx_compiler.h"

#include <macros.h>

#include <fbxsdk.h>
#include <Shlwapi.h>

#include <math/vector.h>

#include <lib/allocator.h>
#include <lib/buffer_array.h>

#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include <lib/linear_set.h>

#include <lib/writer.h>

#include <system/path.h>
#include <system/file.h>
#include <system/file_iterator.h>

namespace resource_compiler {

void fbx_compiler::create( )
{
	m_fbx_manager = FbxManager::Create( );
	m_importer = FbxImporter::Create( m_fbx_manager, "" );
}

void fbx_compiler::destroy( )
{
	m_importer->Destroy( );
	m_fbx_manager->Destroy( );
}

template<typename ElementType>
static inline ElementType get_element( FbxMesh* const mesh, FbxLayerElementTemplate<ElementType>* const element, u32 const polygon_index, u32 const vertex_index )
{
	u32 base_index = 0;

	switch ( element->GetMappingMode( ) )
	{
		case FbxGeometryElement::eByControlPoint:
			base_index = mesh->GetPolygonVertex( polygon_index, vertex_index );
			break;
		case FbxGeometryElement::eByPolygonVertex:
			base_index = polygon_index * 3 + vertex_index;
			break;
		case FbxGeometryElement::eByPolygon:
			base_index = polygon_index;
			break;
		default:
			FATAL_ERROR( );
	}

	if ( element->GetReferenceMode( ) == FbxGeometryElement::eIndexToDirect )
		base_index = element->GetIndexArray( ).GetAt( base_index );

	return element->GetDirectArray( ).GetAt( base_index );
}

struct vertex_data_type
{
	math::float2 uv;
	math::half4 normal;
};

math::float3 get_mesh_vertex( FbxMesh* const mesh, u32 const polygon_index, u32 const vertex_index )
{
	FbxVector4 const& pos = mesh->GetControlPointAt( mesh->GetPolygonVertex( polygon_index, vertex_index ) );
	return math::float3( (float)pos.mData[0], (float)pos.mData[1], (float)pos.mData[2] );
}

vertex_data_type get_mesh_vertex_data( FbxMesh* const mesh, u32 const polygon_index, u32 const vertex_index )
{
	vertex_data_type data;

	FbxVector2 const& uv		= get_element( mesh, mesh->GetElementUV( 0 ), polygon_index, vertex_index );
	FbxVector4 const normal		= get_element( mesh, mesh->GetElementNormal( 0 ), polygon_index, vertex_index );

	data.uv						= math::float2( (float)uv.mData[0], (float)uv.mData[1] );
	data.normal					= math::half4( (float)normal.mData[0], (float)normal.mData[1], (float)normal.mData[2], 0.0f );

	return data;
}

bool operator==( vertex_data_type const& l, vertex_data_type const& r )
{
	return
		( l.uv		== r.uv		) &&
		( l.normal	== r.normal	);
}

static void process_mesh( FbxMesh* const mesh, lib::buffer_array<math::float3>& vertices, lib::buffer_array<vertex_data_type>& vertices_data, u16*& indices, u32& index_count )
{
	lib::linear_set<u16, 256 * 1024> vertices_map;
	vertices_map.create( );

	u32 const polygon_count = mesh->GetPolygonCount( );

	u16* const indices_data = std_allocator( ).allocate( polygon_count * 3 * sizeof(u16) );

	for ( u32 i = 0; i < polygon_count; ++i )
	{
		for ( u32 j = 0; j < 3; ++j )
		{
			math::float3 const& vertex = get_mesh_vertex( mesh, i, j );
			vertex_data_type const& vertex_data = get_mesh_vertex_data( mesh, i, j );

			u16* index;
			vertices_map.find_if_or_insert(
				lib::hash( vertex, lib::hash( vertex_data ) ),
				[&vertex, &vertex_data, &vertices, &vertices_data]( u32 const value )
				{
					return ( vertices[value] == vertex ) && ( vertices_data[value] == vertex_data );
				},
				[&vertex, &vertex_data, &vertices, &vertices_data]( )
				{
					vertices.emplace_back( ) = vertex;
					vertices_data.emplace_back( ) = vertex_data;
					return (u32)( vertices.size( ) - 1 );
				},
				index
			);
			
			indices_data[i * 3 + j] = *index;
		}
	}

	vertices_map.destroy( );

	indices			= indices_data;
	index_count		= polygon_count * 3;
}

static void write_to_file( FbxMesh* const mesh, sys::path const& path )
{
	lib::buffer_array<math::float3> vertices;
	lib::buffer_array<vertex_data_type> vertices_data;

	u32 const max_vertices = 65536;
	std_allocator::scoped_memory const vertex_memory( max_vertices * ( sizeof(math::float3) + sizeof(vertex_data_type) ) );
	pbyte const vertices_memory = vertex_memory;
	pbyte const vertices_data_memory = vertices_memory + max_vertices * sizeof(math::float3);

	vertices.create( vertices_memory, max_vertices );
	vertices_data.create( vertices_data_memory, max_vertices );
	
	u16* indices;
	u32 index_count;

	process_mesh( mesh, vertices, vertices_data, indices, index_count );

	u32 const vertex_count = (u32)vertices.size( );

	uptr const total_size = sizeof(u32) * 2 + index_count * sizeof(u16) + vertices.size( ) * ( sizeof(math::float3) + sizeof(vertex_data_type) );

	std_allocator::scoped_memory const file_data_ptr( total_size );
	pointer const file_data = file_data_ptr;

	lib::writer w( file_data, total_size );

	w.write( index_count );
	w.write( vertex_count );
	w.write_data( indices, index_count * sizeof(u16) );
	w.write_data( vertices.data( ), vertices.size( ) * sizeof(math::float3) );
	w.write_data( vertices_data.data( ), vertices_data.size( ) * sizeof(vertex_data_type) );

	sys::file f;
	f.create( path.c_str( ), sys::file::open_write );
	if ( !f.is_valid( ) )
	{
		LOG( "fbx_compiler: unable to create a file: \"%s\"\n", path.c_str( ) );
		return;
	}

	f.write( file_data, total_size );
	f.destroy( );

	std_allocator( ).deallocate( indices );
}

void fbx_compiler::compile( u64 relevant_date, weak_const_string input_file_name, weak_const_string output_directory )
{
	little_string file_name = sys::path::get_file_name( input_file_name );

	ASSERT( file_name.copy( file_name.length( ) - 4, 4 ) == ".fbx", "wrong file extension: %s", file_name.copy( file_name.length( ) - 4, 4 ) );

	sys::path::remove_file_extension( file_name.data( ) );

	file_name.append( ".mesh" );

	sys::path output_path = str512( output_directory );
	output_path += file_name;

	sys::file_iterator fi;
	fi.create( output_path.c_str( ) );
	if ( fi.is_valid( ) )
		if ( fi.get_modification_time( ) > relevant_date )
		{
			LOG( "fbx_compiler: fbx mesh update is not required: \"%s\"\n", output_path.c_str( ) );
			return;
		}

	bool const status = m_importer->Initialize( input_file_name.c_str( ) );
	if ( !status )
	{
		LOG( "fbx_compiler: importer initialization error: \"%s\"\n", output_path.c_str( ) );
		return;
	}

	FbxScene* scene = FbxScene::Create( m_fbx_manager, "" );
	m_importer->Import( scene );

	FbxNode* root = scene->GetRootNode( );
	FbxMesh* mesh = nullptr;

	for ( u32 i = 0, l = root->GetChildCount( ); i < l; ++i )	
	{
		FbxNode* child = root->GetChild( i );
		mesh = child->GetMesh( );
		if ( mesh )
		{
			root = child;
			break;
		}
	}

	if ( ( mesh == nullptr ) || ( !mesh->IsTriangleMesh( ) ) )
	{
		LOG( "fbx_compiler: bad input file: \"%s\"\n", output_path.c_str( ) );
		return;
	}

	write_to_file( mesh, output_path.c_str( ) );

	scene->Destroy( );

	LOG( "fbx_compiler: fbx mesh compiled successfully: \"%s\"\n", output_path.c_str( ) );
}

} // namespace resource_compiler
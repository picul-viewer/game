#include "fbx_compiler.h"

#include <macros.h>

#include <fbxsdk.h>
#include <Shlwapi.h>

#include <math/vector.h>

#include <lib/allocator.h>
#include <lib/dynamic_array.h>

#include <lib/weak_string.h>
#include <lib/fixed_string.h>

#include <lib/linear_set.h>

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

static inline u32 convert_uv( FbxVector2 const& uv )
{
	return (u32)( uv.mData[0] * 65535.0 + 0.5 ) |
		( (u32)( uv.mData[1] * 65535.0 + 0.5 ) << 16 );
}

static inline u32 convert_normal( FbxVector4& normal )
{
	normal.Normalize( );

	return (u8)( ( normal.mData[0] + 1.0 ) * 127.0 + 0.5 ) |
		( (u8)( ( normal.mData[1] + 1.0f ) * 127.0 + 0.5 ) << 8 ) |
		( (u8)( ( normal.mData[2] + 1.0f ) * 127.0 + 0.5 ) << 16 );
}

mem_align( 4 )
struct vertex
{
	math::float3 pos;
	math::float2 uv;
	math::float3 normal;

	void init( FbxMesh* const mesh, u32 const polygon_index, u32 const vertex_index )
	{
		FbxVector4 const& pos		= mesh->GetControlPointAt( mesh->GetPolygonVertex( polygon_index, vertex_index ) );

		FbxVector2 const& uv		= get_element( mesh, mesh->GetElementUV( 0 ), polygon_index, vertex_index );
		FbxVector4 const normal		= get_element( mesh, mesh->GetElementNormal( 0 ), polygon_index, vertex_index );

		this->pos					= math::float3( (float)pos.mData[0], (float)pos.mData[1], (float)pos.mData[2] );
		this->uv					= math::float2( (float)uv.mData[0], (float)uv.mData[1] );
		this->normal				= math::float3( (float)normal.mData[0], (float)normal.mData[1], (float)normal.mData[2] );
	}
};

bool operator==( vertex const& l, vertex const& r )
{
	return
		( l.pos		== r.pos	) &&
		( l.uv		== r.uv		) &&
		( l.normal	== r.normal	);
}

mem_align( 4 )
struct bumpmapped_vertex : public vertex
{
	math::float3 tangent;
	math::float3 binormal;
	
	void init( FbxMesh* const mesh, u32 const polygon_index, u32 const vertex_index )
	{
		vertex::init( mesh, polygon_index, vertex_index );

		FbxVector4 const tangent	= get_element( mesh, mesh->GetElementTangent( 0 ), polygon_index, vertex_index );
		FbxVector4 const binormal	= get_element( mesh, mesh->GetElementBinormal( 0 ), polygon_index, vertex_index );

		this->tangent				= math::float3( (float)tangent.mData[0], (float)tangent.mData[1], (float)tangent.mData[2] );
		this->binormal				= math::float3( (float)binormal.mData[0], (float)binormal.mData[1], (float)binormal.mData[2] );
	}
};

bool operator==( bumpmapped_vertex const& l, bumpmapped_vertex const& r )
{
	return
		( (vertex)l		== (vertex)r	) &&
		( l.tangent		== r.tangent	) &&
		( l.binormal	== r.binormal	);
}

template<typename VertexType>
static inline bool process_mesh( FbxMesh* const mesh, lib::dynamic_array<VertexType>& vertices, u32*& indices, u32& index_count )
{
	lib::linear_set<u32, 256 * 1024> vertices_map;
	vertices_map.create( );

	u32 current_index = 0;

	u32 const polygon_count = mesh->GetPolygonCount( );

	u32* const indices_data = std_allocator( ).allocate( polygon_count * 3 * sizeof(u32) );

	for ( u32 i = 0; i < polygon_count; ++i )
	{
		for ( u32 j = 0; j < 3; ++j )
		{
			VertexType v;
			v.init( mesh, i, j );

			u32* index;
			vertices_map.find_if_or_insert(
				lib::hash( v ),
				[&v, &vertices]( u32 const value )
				{
					return vertices[value] == v;
				},
				[&v, &vertices]( )
				{
					vertices.emplace_back( ) = v;
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
	
	if ( current_index < 65536 )
	{
		// use 16-bit indices
		for ( u32 i = 0; i < polygon_count * 3 / 2; ++i )
			indices_data[i] = ( indices[i * 2 + 0] & 0x0000FFFF ) | ( indices[i * 2 + 1] << 16 );

		if ( polygon_count & 1 )
			indices_data[polygon_count * 3 / 2] = indices_data[polygon_count * 3 - 1];

		return true;
	}

	return false;
}

template<typename VertexType>
static inline void write_to_file( FbxMesh* const mesh, sys::path const& path, u32 const configure_mask )
{
	lib::dynamic_array<VertexType> vertices;
	vertices.create( nullptr, 32 * Mb );
	
	u32* indices;
	u32 index_count;

	bool const use_16bit_indices = process_mesh( mesh, vertices, indices, index_count );
	if ( index_count >= ( 1u << 29 ) )
	{
		LOG( "fbx_compiler: the mesh is too big: \"%s\"\n", path.c_str( ) );
		return;
	}

	u32 const indices_size = index_count * ( use_16bit_indices ? sizeof(u16) : sizeof(u32) );
	u32 const vertices_size = (u32)( vertices.size( ) * sizeof(VertexType) );

	u32 const total_size = sizeof(u32) + indices_size + sizeof(u32) + vertices_size;

	pointer const file_data = std_allocator( ).allocate( total_size );
	
	u32 index_data = index_count;
	index_data |= configure_mask;

	if ( !use_16bit_indices )
		index_data |= ( 1u << 29 );

	memcpy( file_data, &index_data, sizeof(u32) );
	memcpy( file_data + sizeof(u32), indices, indices_size );
	memcpy( file_data + sizeof(u32) + indices_size, &vertices_size, sizeof(u32) );
	memcpy( file_data + sizeof(u32) + indices_size + sizeof(u32), vertices.data( ), vertices_size );

	sys::file f;
	f.create( path.c_str( ), sys::file::open_write );
	if ( !f.is_valid( ) )
	{
		LOG( "fbx_compiler: unable to create a file: \"%s\"\n", path.c_str( ) );
		return;
	}

	f.write( file_data, total_size );
	f.destroy( );

	vertices.destroy( );
	std_allocator( ).deallocate( indices );
	std_allocator( ).deallocate( file_data );
}

void fbx_compiler::compile( u64 relevant_date, weak_const_string input_file_name, weak_const_string output_directory )
{
	little_string file_name = sys::path::get_file_name( input_file_name );

#ifdef DEBUG
	ASSERT( file_name.copy( file_name.length( ) - 4, 4 ) == ".fbx", "wrong file extension: %s", file_name.copy( file_name.length( ) - 4, 4 ) );
#endif // #ifdef DEBUG
	
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
	
	bool const is_bumpmapped = mesh->GetElementTangentCount( ) && mesh->GetElementBinormalCount( );

	if ( is_bumpmapped )
		write_to_file<bumpmapped_vertex>( mesh, output_path.c_str( ), 0x80000000 );
	else
		write_to_file<vertex>( mesh, output_path.c_str( ), 0x00000000 );

	scene->Destroy( );

	LOG( "fbx_compiler: fbx mesh compiled successfully: \"%s\"\n", output_path.c_str( ) );
}

} // namespace resource_compiler
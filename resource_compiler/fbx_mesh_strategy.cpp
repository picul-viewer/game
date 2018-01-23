#include "fbx_mesh_strategy.h"

#include <fbxsdk.h>
#include <map>
#include <Shlwapi.h>

#include <core/math.h>

namespace resource_compiler {

void fbx_mesh_strategy::on_start( )
{
	m_fbx_manager = FbxManager::Create( );
	m_importer = FbxImporter::Create( m_fbx_manager, "" );
}

void fbx_mesh_strategy::on_finish( )
{
	m_importer->Destroy( );
	m_fbx_manager->Destroy( );
}

template<typename ElementType>
static inline ElementType get_element( FbxMesh* mesh, FbxLayerElementTemplate<ElementType>* element, u32 polygon_index, u32 vertex_index )
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
	default: FATAL_ERROR( )
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
	float pos_x, pos_y, pos_z;
	u32 uv;
	u32 normal;

	void init( FbxMesh* mesh, u32 polygon_index, u32 vertex_index )
	{
		FbxVector4 const& position	= mesh->GetControlPointAt( mesh->GetPolygonVertex( polygon_index, vertex_index ) );

		FbxVector2 const& uv		= get_element( mesh, mesh->GetElementUV( 0 ), polygon_index, vertex_index );
		FbxVector4 normal			= get_element( mesh, mesh->GetElementNormal( 0 ), polygon_index, vertex_index );

		this->pos_x					= (float)position.mData[0];
		this->pos_y					= (float)position.mData[1];
		this->pos_z					= (float)position.mData[2];
		this->uv					= convert_uv( uv );
		this->normal				= convert_normal( normal );
	}
};

bool operator<( vertex const& l, vertex const& r )
{
	if ( l.pos_x != r.pos_x )
		return l.pos_x < r.pos_x;
	if ( l.pos_y != r.pos_y )
		return l.pos_y < r.pos_y;
	if ( l.pos_z != r.pos_z )
		return l.pos_z < r.pos_z;

	if ( l.uv != r.uv )
		return l.uv < r.uv;

	return l.normal < r.normal;
}

mem_align( 4 )
struct bumpmapped_vertex : public vertex
{
	u32 tangent;
	u32 binormal;
	
	void init( FbxMesh* mesh, u32 polygon_index, u32 vertex_index )
	{
		vertex::init( mesh, polygon_index, vertex_index );

		FbxVector4 tangent	= get_element( mesh, mesh->GetElementTangent( 0 ), polygon_index, vertex_index );
		FbxVector4 binormal	= get_element( mesh, mesh->GetElementBinormal( 0 ), polygon_index, vertex_index );

		this->tangent		= convert_normal( tangent );
		this->binormal		= convert_normal( binormal );
	}
};

bool operator<( bumpmapped_vertex const& l, bumpmapped_vertex const& r )
{
	if ( l.normal != r.normal )
		return (vertex)l < (vertex)r;

	if ( l.tangent != r.tangent )
		return l.tangent < r.tangent;
	return l.binormal < r.binormal;
}

template<typename VertexType>
static inline bool process_mesh( FbxMesh* mesh, VertexType*& vertices, u32*& indices, u32& vertex_count, u32& index_count )
{
	map<VertexType, u32> vertices_map;
	u32 current_index = 0;

	u32 polygon_count = mesh->GetPolygonCount( );

	u32* indices_data = mem_allocator( ).allocate( polygon_count * 3 * sizeof(u32) );

	for ( u32 i = 0; i < polygon_count; ++i )
	{
		for ( u32 j = 0; j < 3; ++j )
		{
			VertexType v;
			v.init						( mesh, i, j );

			auto it						= vertices_map.find( v );
			if ( it != vertices_map.end( ) )
				indices_data[i * 3 + j]	= it->second;
			else
			{
				vertices_map[v]			= current_index;
				indices_data[i * 3 + j]	= current_index;
				++current_index;
			}
		}
	}

	VertexType* vertices_data			= mem_allocator( ).allocate( current_index * sizeof(VertexType) );

	for ( auto i = vertices_map.begin( ), l = vertices_map.end( ); i != l; ++i )
		vertices_data[i->second]		= i->first;

	vertices		= vertices_data;
	indices			= indices_data;
	vertex_count	= current_index;
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
static inline void write_to_file( FbxMesh* mesh, weak_const_string path, u32 configure_mask )
{
	VertexType* vertices;
	u32* indices;
	u32 vertex_count;
	u32 index_count;

	bool use_16bit_indices = process_mesh( mesh, vertices, indices, vertex_count, index_count );

	ASSERT( index_count < ( 1u << 29 ), "the mesh is too big" );

	u32 indices_size = index_count * ( use_16bit_indices ? sizeof(u16) : sizeof(u32) );
	u32 vertices_size = vertex_count * sizeof(VertexType);

	u32 total_size = sizeof(u32) + indices_size + sizeof(u32) + vertices_size;

	pointer file_data = mem_allocator( ).allocate( total_size );
	
	u32 index_data = index_count;
	index_data |= configure_mask;

	if ( use_16bit_indices )
		index_data |= ( 1u << 29 );

	memcpy( file_data, &index_data, sizeof(u32) );
	memcpy( file_data + sizeof(u32), indices, indices_size );
	memcpy( file_data + sizeof(u32) + indices_size, &vertices_size, sizeof(u32) );
	memcpy( file_data + sizeof(u32) + indices_size + sizeof(u32), vertices, vertices_size );

	HANDLE file = CreateFile( path.c_str( ), GENERIC_WRITE | DELETE, 0, nullptr, CREATE_ALWAYS, 0, nullptr );

	DWORD bytes_write = 0;
	WriteFile( file, file_data, total_size, &bytes_write, nullptr );
	ASSERT( bytes_write == total_size );

	CloseHandle( file );

	mem_allocator( ).deallocate( vertices );
	mem_allocator( ).deallocate( indices );
	mem_allocator( ).deallocate( file_data );
}

void fbx_mesh_strategy::compile( u64 relevant_date, weak_const_string input_path, weak_const_string output_directory )
{
	str512 file_name = PathFindFileName( input_path.c_str( ) );

#ifdef DEBUG
	ASSERT( file_name.copy( file_name.length( ) - 4, 4 ) == weak_const_string( ".fbx" ) );
#endif // #ifdef DEBUG
	
	PathRemoveExtension( file_name.data( ) );

	file_name.append( weak_const_string( ".mesh" ) );

	str512 output_path = str512( output_directory );
	PathAppend( output_path.data( ), file_name.c_str( ) );
	
	WIN32_FIND_DATA output_data;
	if ( FindFirstFile( output_path.c_str( ), &output_data ) != INVALID_HANDLE_VALUE )
		if ( filetime_to_u64( output_data.ftLastWriteTime ) > relevant_date )
			return;

	bool status = m_importer->Initialize( input_path.c_str( ) );
	ASSERT( status );

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

	ASSERT( mesh != nullptr );

	ASSERT( mesh->IsTriangleMesh( ), "only triangle meshes are supported" );
	
	bool is_bumpmapped = mesh->GetElementTangentCount( ) && mesh->GetElementBinormalCount( );

	if ( is_bumpmapped )
		write_to_file<bumpmapped_vertex>( mesh, output_path.c_str( ), 0x80000000 );
	else
		write_to_file<vertex>( mesh, output_path.c_str( ), 0x00000000 );

	scene->Destroy( );
}

} // namespace resource_compiler
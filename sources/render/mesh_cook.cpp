#include "mesh_cook.h"

#include <lib/allocator.h>
#include <lib/hash.h>
#include <lib/reader.h>
#include <lib/strings.h>
#include <resources/raw_data.h>

#include "mesh.h"
#include "input_layout.h"

#include "mesh_box.h"
#include "mesh_quad.h"

namespace render {

mesh_cook* mesh_cook::create( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	uptr const size = sizeof(mesh_cook) + length + 1;

	mesh_cook* const result = std_allocator( ).allocate( sizeof(mesh_cook) + size );

	u32 const id = lib::hash32( in_path, length );
	result->init( id );

	result->m_length = length;
	strings::copy_n( result->m_path, in_path, size );

	return result;
}

void mesh_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void mesh_cook::create_resource( )
{
	check_resource_exists( &mesh_cook::query_file );
}

void mesh_cook::query_file( mesh* const in_resource_ptr )
{
	m_result = in_resource_ptr;

	// Check for some defaut meshes
	if ( strings::equal( m_path, "box" ) )
	{
		u32 const stride = sizeof(math::float3);

		buffer::cook vertex_cook;
		vertex_cook.set_vertex_buffer( sizeof(box::vertices) );
		m_result->create_vertex_buffer( 0, vertex_cook, box::vertices, stride );

		buffer::cook index_cook;
		index_cook.set_index_buffer( sizeof(box::indices) );
		m_result->create_index_buffer( index_cook, box::indices, DXGI_FORMAT_R16_UINT );

		m_result->set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_result->set_dimensions( box::index_count );

		finish( m_result );
		return;
	}
	
	if ( strings::equal( m_path, "quad" ) )
	{
		u32 const stride = sizeof(math::float3);

		buffer::cook vertex_cook;
		vertex_cook.set_vertex_buffer( sizeof(quad::vertices) );
		m_result->create_vertex_buffer( 0, vertex_cook, quad::vertices, stride );

		buffer::cook index_cook;
		index_cook.set_index_buffer( sizeof(quad::indices) );
		m_result->create_index_buffer( index_cook, quad::indices, DXGI_FORMAT_R16_UINT );

		m_result->set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_result->set_dimensions( quad::index_count );

		finish( m_result );
		return;
	}

	raw_data_cook* const cook = raw_data_cook::create( m_path );

	create_child_resources( callback_task<&mesh_cook::on_file_loaded>( ), cook );
}

void mesh_cook::on_file_loaded( queried_resources& in_queried )
{
	raw_data::ptr const data = in_queried.get_resource<raw_data::ptr>( );
	ASSERT( data != nullptr );

	lib::reader r( data->data( ), data->size( ) );


	u32 const index_data				= r.read<u32>( );
	
	// 29 LSBs - indices count
	m_result->m_index_count				= index_data & 0x1FFFFFFF;
	// Index format is coded by 29st bit in index count
	m_result->m_index_format			= ( index_data & 0x20000000 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

	u32 const indices_size				= m_result->m_index_count * format_get_bits_per_pixel( m_result->m_index_format ) / 8;
	buffer::cook cook;
	cook.set_index_buffer				( indices_size );
	m_result->m_index_buffer.create		( cook, r.read_data( indices_size ) );

	m_result->m_primitive_topology		= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	static const vertex_type vertex_formats[] = {
		vertex_type::vertex_type_mesh,
	};

	// Vertex type can be uncoded by 2 MSBs
	u32 const vertex_format_id			= index_data >> 30;
	ASSERT_CMP							( vertex_format_id, <, ARRAYSIZE( vertex_formats ) );
	vertex_type const vertex			= vertex_formats[vertex_format_id];
	u32 const buffers_count				= get_vertex_type_buffers_count( vertex );

	for ( u32 i = 0; i < buffers_count; ++i )
	{
		m_result->m_vertex_strides[i]	= get_vertex_type_size( i, vertex );

		u32 const vertices_size			= r.read<u32>( );
		buffer::cook cook;
		cook.set_vertex_buffer			( vertices_size );
		m_result->m_vertex_buffers[i].create( cook, r.read_data( vertices_size ) );
	}


	finish( m_result );
}

} // namespace render
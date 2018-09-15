#include "mesh.h"

#include "render_api.h"
#include "input_layout.h"

#include <macros.h>
#include <lib/weak_string.h>
#include <lib/allocator.h>

#include <system/file.h>

namespace render
{

void mesh::create( binary_config& in_config )
{
	ASSERT( in_config.is_valid( ) );

	const u32 index_data				= in_config.read<u32>( );
	
	// 29 LSBs - indices count
	m_index_count						= index_data & 0x1FFFFFFF;
	m_instance_count					= 1;
	// Index format is coded by 29st bit in index count
	m_index_format						= ( index_data & 0x20000000 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

	u32 indices_size					= m_index_count * format_get_bits_per_pixel( m_index_format );
	buffer::cook cook;
	cook.set_index_buffer				( indices_size );
	m_index_buffer.create				( cook, in_config.read_data( indices_size ) );

	m_primitive_topology				= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	static const vertex_type vertex_formats[] = {
		vertex_type::vertex_type_mesh,
	};

	// Vertex type can be uncoded by 2 MSBs
	u32 vertex_format_id				= index_data >> 30;
	ASSERT								( vertex_format_id < ARRAYSIZE( vertex_formats ) );
	vertex_type vertex					= vertex_formats[vertex_format_id];
	u32 buffers_count					= get_vertex_type_buffers_count( vertex );

	for ( u32 i = 0; i < buffers_count; ++i )
	{
		m_vertex_strides[i]				= get_vertex_type_size( i, vertex );

		u32 vertices_size				= in_config.read<u32>( );
		buffer::cook cook;
		cook.set_vertex_buffer			( vertices_size );
		m_vertex_buffers[i].create		( cook, in_config.read_data( vertices_size ) );
	}
}

u32 mesh::add_ref( ) const
{
	for ( u32 i = 0; i < max_vertex_buffers_count; ++i )
		if ( m_vertex_buffers[i].get( ) )
			m_vertex_buffers[i].add_ref( );
	return m_index_buffer.add_ref( );
}

u32 mesh::release( ) const
{
	for ( u32 i = 0; i < max_vertex_buffers_count; ++i )
		if ( m_vertex_buffers[i].get( ) )
			m_vertex_buffers[i].release( );
	return m_index_buffer.release( );
}

void mesh::destroy( )
{
	for ( u32 i = 0; i < max_vertex_buffers_count; ++i )
		m_vertex_buffers[i].destroy( );
	m_index_buffer.destroy( );
}

void mesh::set_registry_pointer( pointer in_pointer )
{
	m_registry_pointer = in_pointer;
}

pointer mesh::get_registry_pointer( ) const
{
	return m_registry_pointer;
}

buffer const& mesh::get_vertex_buffer( u32 in_index ) const
{
	ASSERT( in_index < max_vertex_buffers_count );
	return m_vertex_buffers[in_index];
}

buffer const& mesh::get_index_buffer( ) const
{
	return m_index_buffer;
}

void mesh::bind( ) const
{
	static const u32 null_vertex_offsets[max_vertex_buffers_count] = { };
	
	g_api.get_context( )->IASetVertexBuffers( 0, max_vertex_buffers_count, (ID3D11Buffer**)m_vertex_buffers, m_vertex_strides, null_vertex_offsets );
	g_api.get_context( )->IASetIndexBuffer( *(ID3D11Buffer**)&m_index_buffer, m_index_format, 0 );
	g_api.get_context( )->IASetPrimitiveTopology( m_primitive_topology );
}

void mesh::draw( ) const
{
	bind( );
	g_api.get_context( )->DrawIndexed( m_index_count, 0, 0 );
}

void mesh::draw( u32 in_index_count ) const
{
	bind( );
	g_api.get_context( )->DrawIndexed( in_index_count, 0, 0 );
}

void mesh::draw_instanced( ) const
{
	bind( );
	g_api.get_context( )->DrawIndexedInstanced( m_index_count, m_instance_count, 0, 0, 0 );
}

void mesh::draw_instanced( u32 in_instance_count ) const
{
	bind( );
	g_api.get_context( )->DrawIndexedInstanced( m_index_count, in_instance_count, 0, 0, 0 );
}

void mesh::create_vertex_buffer( u32 in_index, buffer::cook const& in_cook, pcvoid in_data, u32 in_stride )
{
	ASSERT( in_index < max_vertex_buffers_count );
	
	ASSERT( in_cook.buffer_desc.ByteWidth % in_stride == 0 );
	
	m_vertex_buffers[in_index].create( in_cook, in_data );
	m_vertex_strides[in_index] = in_stride;
}

void mesh::create_index_buffer( buffer::cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format )
{
	ASSERT( ( in_format == DXGI_FORMAT_R16_UINT ) || ( in_format == DXGI_FORMAT_R32_UINT ) );
	
	u32 index_size = ( in_format == DXGI_FORMAT_R16_UINT ) ? 2 : 4;
	
	ASSERT( in_cook.buffer_desc.ByteWidth % index_size == 0 );
	m_index_count = in_cook.buffer_desc.ByteWidth / index_size;
	
	m_index_buffer.create( in_cook, in_data );
	m_index_format = in_format;
}

void mesh::set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY in_primitive_topology )
{
	m_primitive_topology = in_primitive_topology;
}

void mesh::set_dimensions( u32 in_index_count, u32 in_instance_count )
{
	m_index_count		= in_index_count;
	m_instance_count	= in_instance_count;
}

} // namespace render
#include "mesh.h"

#include "render_api.h"
#include "input_layout.h"

namespace render
{

void mesh::create( pointer in_data, uptr in_size )
{
	ASSERT						( in_size < 3 * sizeof(u32) );

	const vertex_type vertex	= (vertex_type)*( (u32*)in_data + 0 );
	const u32 index_count		= (vertex_type)*( (u32*)in_data + 1 );
	m_instance_count			= (vertex_type)*( (u32*)in_data + 2 );
	
	// Index format is coded by highest bit in index count
	m_index_format				= ( index_count & 0x80000000 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
	m_index_count				= ( index_count & 0x7FFFFFFF );

	set_vertex_buffers_count	( get_vertex_type_buffers_count( vertex ) );

	pointer ptr					= in_data + 3 * sizeof(u32);
	
	for ( u32 i = 0; i < m_buffers_count; ++i )
	{
		ASSERT					( ptr + sizeof(u32) < in_data + in_size );

		const u32 buffer_size	= ptr.get<u32>( );
		ptr						+= sizeof(u32);
		
		buffer::cook cook;
		cook.set_vertex_buffer	( buffer_size );
		
		m_vertex_strides[i]		= get_vertex_type_size( i, vertex );
		
		ASSERT					( ptr + buffer_size < in_data + in_size );

		create_vertex_buffer	( i, cook, ptr, m_vertex_strides[i] );
		ptr						+= buffer_size;
	}

	buffer::cook cook;
	cook.set_index_buffer		( m_index_count );

	ASSERT						( ptr + m_index_count < in_data + in_size );

	create_index_buffer			( cook, ptr, m_index_format );

	m_primitive_topology		= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void mesh::set_vertex_buffers_count( u32 in_vertex_buffers_count )
{
	ASSERT			( in_vertex_buffers_count < max_vertex_buffers_count );
	m_buffers_count	= in_vertex_buffers_count;
}

void mesh::create_vertex_buffer( u32 in_index, buffer::cook const& in_cook, pcvoid in_data, u32 in_stride )
{
	ASSERT( in_index < m_buffers_count );
	
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

void mesh::add_ref( ) const
{
	for ( u32 i = 0; i < m_buffers_count; ++i )
		m_vertex_buffers[i].add_ref( );
	m_index_buffer.add_ref( );
}

void mesh::release( ) const
{
	for ( u32 i = 0; i < m_buffers_count; ++i )
		m_vertex_buffers[i].release( );
	m_index_buffer.release( );
}

void mesh::destroy( )
{
	for ( u32 i = 0; i < m_buffers_count; ++i )
		m_vertex_buffers[i].destroy( );
	m_index_buffer.destroy( );
}

buffer const& mesh::get_vertex_buffer( u32 in_index ) const
{
	ASSERT( in_index < m_buffers_count );
	return m_vertex_buffers[in_index];
}

buffer const& mesh::get_index_buffer( ) const
{
	return m_index_buffer;
}

void mesh::bind( ) const
{
	static const u32 null_vertex_offsets[max_vertex_buffers_count] = { };
	
	api::get_context( )->IASetVertexBuffers( 0, max_vertex_buffers_count, (ID3D11Buffer**)m_vertex_buffers, m_vertex_strides, null_vertex_offsets );
	api::get_context( )->IASetIndexBuffer( *(ID3D11Buffer**)&m_index_buffer, m_index_format, 0 );
	api::get_context( )->IASetPrimitiveTopology( m_primitive_topology );
}

void mesh::draw( ) const
{
	bind( );
	api::get_context( )->DrawIndexed( m_index_count, 0, 0 );
}

void mesh::draw( u32 in_index_count ) const
{
	bind( );
	api::get_context( )->DrawIndexed( in_index_count, 0, 0 );
}

void mesh::draw_instanced( ) const
{
	bind( );
	api::get_context( )->DrawIndexedInstanced( m_index_count, m_instance_count, 0, 0, 0 );
}

void mesh::draw_instanced( u32 in_instance_count ) const
{
	bind( );
	api::get_context( )->DrawIndexedInstanced( m_index_count, in_instance_count, 0, 0, 0 );
}

} // namespace render
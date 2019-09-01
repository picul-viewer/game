#include "mesh.h"

#include "api.h"
#include "input_layout.h"

#include <macros.h>
#include <lib/weak_string.h>
#include <lib/allocator.h>

#include <system/file.h>

namespace render {

void mesh::destroy_resource( mesh* const in_resource )
{
	for ( u32 i = 0; i < mesh::max_vertex_buffers_count; ++i )
		in_resource->m_vertex_buffers[i].destroy( );
	in_resource->m_index_buffer.destroy( );

	container( ).remove( in_resource );
}

buffer const& mesh::get_vertex_buffer( u32 in_index ) const
{
	ASSERT_CMP( in_index, <, max_vertex_buffers_count );
	return m_vertex_buffers[in_index];
}

buffer const& mesh::get_index_buffer( ) const
{
	return m_index_buffer;
}

DXGI_FORMAT mesh::get_index_format( ) const
{
	return m_index_format;
}

D3D11_PRIMITIVE_TOPOLOGY mesh::get_primitive_topology( ) const
{
	return m_primitive_topology;
}

u32 mesh::get_index_count( ) const
{
	return m_index_count;
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

void mesh::draw_instanced( u32 in_instance_count ) const
{
	bind( );
	g_api.get_context( )->DrawIndexedInstanced( m_index_count, in_instance_count, 0, 0, 0 );
}

void mesh::create_vertex_buffer( u32 in_index, buffer::cook const& in_cook, pcvoid in_data, u32 in_stride )
{
	ASSERT_CMP( in_index, <, max_vertex_buffers_count );
	
	ASSERT_CMP( in_cook.buffer_desc.ByteWidth % in_stride, ==, 0 );
	
	m_vertex_buffers[in_index].create( in_cook, in_data );
	m_vertex_strides[in_index] = in_stride;
}

void mesh::create_index_buffer( buffer::cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format )
{
	ASSERT( ( in_format == DXGI_FORMAT_R16_UINT ) || ( in_format == DXGI_FORMAT_R32_UINT ) );
	
	u32 index_size = ( in_format == DXGI_FORMAT_R16_UINT ) ? 2 : 4;
	
	ASSERT_CMP( in_cook.buffer_desc.ByteWidth % index_size, ==, 0 );
	m_index_count = in_cook.buffer_desc.ByteWidth / index_size;
	
	m_index_buffer.create( in_cook, in_data );
	m_index_format = in_format;
}

void mesh::set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY in_primitive_topology )
{
	m_primitive_topology = in_primitive_topology;
}

void mesh::set_dimensions( u32 in_index_count )
{
	m_index_count		= in_index_count;
}

} // namespace render
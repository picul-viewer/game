#include "buffer.h"
#include "render_api.h"
#include "input_layout.h"

namespace render {

buffer::buffer( ) :
	m_buffer( nullptr )
{ }

void buffer::cook::set_vertex_buffer( u32	in_size,
									  bool	in_is_dynamic )
{
	buffer_desc.ByteWidth			= in_size;
	buffer_desc.MiscFlags			= 0;
	buffer_desc.StructureByteStride	= 0;

	buffer_desc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;

	buffer_desc.CPUAccessFlags		= in_is_dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	buffer_desc.Usage				= in_is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
}

void buffer::cook::set_index_buffer( u32 in_size )
{
	buffer_desc.ByteWidth			= in_size;
	buffer_desc.MiscFlags			= 0;
	buffer_desc.StructureByteStride	= 0;

	buffer_desc.BindFlags			= D3D11_BIND_INDEX_BUFFER;

	buffer_desc.CPUAccessFlags		= 0;
	buffer_desc.Usage				= D3D11_USAGE_IMMUTABLE;
}

void buffer::cook::set_constant_buffer( u32		in_size,
										bool	in_is_dynamic )
{
	buffer_desc.ByteWidth			= in_size;
	buffer_desc.MiscFlags			= 0;
	buffer_desc.StructureByteStride	= 0;

	buffer_desc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;

	buffer_desc.CPUAccessFlags		= in_is_dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	buffer_desc.Usage				= in_is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
}

void buffer::cook::set_buffer( u32			in_size,
							   D3D11_USAGE	in_usage,
							   u32			in_bind_flags,
							   u32			in_cpu_access_flags,
							   u32			in_misc_flags,
							   u32			in_structure_buffer_size )
{
	buffer_desc.ByteWidth			= in_size;
	buffer_desc.Usage				= in_usage;
	buffer_desc.BindFlags			= in_bind_flags;
	buffer_desc.CPUAccessFlags		= in_cpu_access_flags;
	buffer_desc.MiscFlags			= in_misc_flags;
	buffer_desc.StructureByteStride	= in_structure_buffer_size;
}

void buffer::create( cook const& in_cook, pcvoid in_data )
{
	D3D11_SUBRESOURCE_DATA subresource_data = { in_data };
	api::get_device( )->CreateBuffer( &in_cook.buffer_desc, &subresource_data, &m_buffer );
}

void buffer::create( cook const& in_cook )
{
	api::get_device( )->CreateBuffer( &in_cook.buffer_desc, nullptr, &m_buffer );
}

void buffer::set( ID3D11Buffer* in_buffer )
{
	m_buffer = in_buffer;
}

void buffer::add_ref( ) const
{
	m_buffer->AddRef( );
}

void buffer::release( ) const
{
	m_buffer->Release( );
}

void buffer::destroy( )
{
	dx_release( m_buffer );
}

void buffer::update_default( pcvoid in_data ) const
{
	api::get_context( )->UpdateSubresource( m_buffer, 0, nullptr, in_data, 0, 0 );
}

void buffer::update_default( pcvoid in_data, u32 in_offset, u32 in_size ) const
{
	D3D11_BOX box;
	box.left	= in_offset;
	box.right	= in_offset + in_size;
	box.top		= 0;
	box.bottom	= 1;
	box.front	= 0;
	box.back	= 1;
	api::get_context( )->UpdateSubresource( m_buffer, 0, &box, in_data, 0, 0 );
}

void buffer::update_dynamic( pcvoid in_data, u32 in_offset, u32 in_size ) const
{
	D3D11_MAPPED_SUBRESOURCE mapped_resource{ };
	api::get_context( )->Map( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource );

	memcpy( (s8*)mapped_resource.pData + in_offset, in_data, in_size );

	api::get_context( )->Unmap( m_buffer, 0 );
}


void vertex_buffer::create( cook const& in_cook, pcvoid in_data, u32 in_stride )
{
	buffer::create( in_cook, in_data );
	m_stride = in_stride;
}

void vertex_buffer::bind( u32 in_slot, u32 in_offset ) const
{
	api::get_context( )->IASetVertexBuffers( in_slot, 1, &m_buffer, &m_stride, &in_offset );
}


void index_buffer::create( cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format )
{
	buffer::create( in_cook, in_data );
	m_format = in_format;
}
	
void index_buffer::bind( u32 in_offset ) const
{
	api::get_context( )->IASetIndexBuffer( m_buffer, m_format, in_offset );
}


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
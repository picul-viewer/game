#include "buffer.h"
#include "render_api.h"

namespace render {

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

buffer::buffer( ) :
	m_buffer( nullptr )
{ }

void buffer::create( cook const& in_cook, pcvoid in_data )
{
	D3D11_SUBRESOURCE_DATA subresource_data = { in_data };
	g_api.get_device( )->CreateBuffer( &in_cook.buffer_desc, &subresource_data, &m_buffer );
}

void buffer::create( cook const& in_cook )
{
	g_api.get_device( )->CreateBuffer( &in_cook.buffer_desc, nullptr, &m_buffer );
}

void buffer::destroy( )
{
	dx_release( m_buffer );
}

u32 buffer::add_ref( ) const
{
	return m_buffer->AddRef( );
}

u32 buffer::release( ) const
{
	return m_buffer->Release( );
}

void buffer::update_default( pcvoid in_data ) const
{
	g_api.get_context( )->UpdateSubresource( m_buffer, 0, nullptr, in_data, 0, 0 );
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
	g_api.get_context( )->UpdateSubresource( m_buffer, 0, &box, in_data, 0, 0 );
}

void buffer::update_dynamic( pcvoid in_data, u32 in_offset, u32 in_size ) const
{
	D3D11_MAPPED_SUBRESOURCE mapped_resource{ };
	g_api.get_context( )->Map( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource );

	memcpy( (s8*)mapped_resource.pData + in_offset, in_data, in_size );

	g_api.get_context( )->Unmap( m_buffer, 0 );
}


void vertex_buffer::create( cook const& in_cook, pcvoid in_data, u32 in_stride )
{
	buffer::create( in_cook, in_data );
	m_stride = in_stride;
}

void vertex_buffer::bind( u32 in_slot, u32 in_offset ) const
{
	g_api.get_context( )->IASetVertexBuffers( in_slot, 1, &m_buffer, &m_stride, &in_offset );
}


void index_buffer::create( cook const& in_cook, pcvoid in_data, DXGI_FORMAT in_format )
{
	buffer::create( in_cook, in_data );
	m_format = in_format;
}
	
void index_buffer::bind( u32 in_offset ) const
{
	g_api.get_context( )->IASetIndexBuffer( m_buffer, m_format, in_offset );
}

} // namespace render
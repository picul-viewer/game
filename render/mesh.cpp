#include "mesh.h"

#include "render_api.h"
#include "input_layout.h"

#include <fs/fs_core.h>
#include <render/render_core.h>

namespace render
{

mesh::mesh( ) :
	m_vertex_strides	{ },
	m_index_format		( DXGI_FORMAT_UNKNOWN ),
	m_index_count		( 0 ),
	m_instance_count	( 0 ),
	m_buffers_count		( 0 ),
	m_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED )
{ }

u32 mesh::add_ref( ) const
{
	for ( u32 i = 0; i < m_buffers_count; ++i )
		m_vertex_buffers[i].add_ref( );
	return m_index_buffer.add_ref( );
}

u32 mesh::release( ) const
{
	for ( u32 i = 0; i < m_buffers_count; ++i )
		m_vertex_buffers[i].release( );
	return m_index_buffer.release( );
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


void mesh_creator::create( mesh* out_resource, weak_const_string in_filename )
{
	fs::core::read_file( in_filename.c_str( ), &on_read_from_file, out_resource );
}

void mesh_creator::on_read_from_file( pointer in_data, uptr in_size, mesh* out_mesh )
{
	core::get_device_queue( ).push( &on_read_from_file_command, in_data, in_size, out_mesh );
}

void mesh_creator::on_read_from_file_command( pointer in_data, uptr in_size, mesh* out_mesh )
{
	pointer local_data				= in_data;
	uptr local_size					= in_size;

	ASSERT							( local_size >= sizeof(u32) );

	const u32 index_data			= local_data.get<u32>( );
	
	local_data						+= sizeof(u32);
	local_size						-= sizeof(u32);

	// 29 LSBs - indices count
	out_mesh->m_index_count			= index_data & 0x1FFFFFFF;
	out_mesh->m_instance_count		= 1;
	// Index format is coded by 29st bit in index count
	out_mesh->m_index_format		= ( index_data & 0x20000000 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

	uptr indices_size				= out_mesh->m_index_count * format_get_bits_per_pixel( out_mesh->m_index_format );

	ASSERT							( local_size >= indices_size );

	buffer::cook cook;
	cook.set_index_buffer			( indices_size );

	out_mesh->m_index_buffer.create	( cook, local_data );

	local_data						+= indices_size;
	local_size						-= indices_size;

	out_mesh->m_primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	static const vertex_type vertex_formats[] = {
		vertex_type::vertex_type_mesh,
	};

	// Vertex type can be uncoded by 2 MSBs
	vertex_type vertex			= vertex_formats[index_data >> 30];

	u32 buffers_count			= get_vertex_type_buffers_count( vertex );
	out_mesh->m_buffers_count	= buffers_count;

	for ( u32 i = 0; i < buffers_count; ++i )
	{
		out_mesh->m_vertex_strides[i] = get_vertex_type_size( i, vertex );

		ASSERT					( local_size >= sizeof(u32) );

		u32 vertices_size		= local_data.get<u32>( );

		local_data				+= sizeof(u32);
		local_size				-= sizeof(u32);

		ASSERT					( local_size >= vertices_size );
		
		buffer::cook cook;
		cook.set_vertex_buffer	( vertices_size );
		
		out_mesh->m_vertex_buffers[i].create( cook, local_data );
		
		local_data				+= vertices_size;
		local_size				-= vertices_size;
	}
}


} // namespace render
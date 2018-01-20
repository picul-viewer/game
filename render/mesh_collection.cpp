#include "mesh_collection.h"

#include "mesh_box.h"
#include "mesh_quad.h"

namespace render {

namespace mesh_collection {

mesh m_data[mesh_type_count];

void create( )
{
	static const u32 stride = sizeof(math::float3);
	
	// Box
	{
		buffer::cook vertex_cook;
		vertex_cook.set_vertex_buffer( sizeof(box::vertices) );
		m_data[mesh_type_box].create_vertex_buffer( 0, vertex_cook, box::vertices, stride );

		buffer::cook index_cook;
		vertex_cook.set_index_buffer( sizeof(box::indices) );
		m_data[mesh_type_box].create_index_buffer( index_cook, box::indices, DXGI_FORMAT_R16_UINT );

		m_data[mesh_type_box].set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_data[mesh_type_box].set_dimensions( box::index_count, 0 );
	}

	///////

	// Quad
	{
		buffer::cook vertex_cook;
		vertex_cook.set_vertex_buffer( sizeof(quad::vertices) );
		m_data[mesh_type_quad].create_vertex_buffer( 0, vertex_cook, quad::vertices, stride );

		buffer::cook index_cook;
		vertex_cook.set_index_buffer( sizeof(quad::indices) );
		m_data[mesh_type_quad].create_index_buffer( index_cook, quad::indices, DXGI_FORMAT_R16_UINT );

		m_data[mesh_type_quad].set_primitive_topology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_data[mesh_type_quad].set_dimensions( quad::index_count, 0 );
	}
}

void destroy( )
{
	for ( u32 i = 0; i < mesh_type_count; ++i )
		m_data[i].release( );
}

mesh* get( mesh_type in_mesh_type )
{
	ASSERT( ( in_mesh_type >= 0 ) && ( in_mesh_type < mesh_type_count ) );
	return m_data + in_mesh_type;
}

} // namespace mesh_collection

} // namespace render
#include "mesh.h"
#include <macros.h>
#include "resources.h"

namespace render {

void mesh::destroy_resource( mesh* const in_resource )
{
	ASSERT( in_resource->m_index_buffer_offset != -1 );
	ASSERT( in_resource->m_vertex_buffer_offset != -1 );

	g_resources.destroy_mesh_index_buffer( in_resource->m_index_buffer_offset, in_resource->m_index_count );
	g_resources.destroy_mesh_vertex_buffer( in_resource->m_vertex_buffer_offset, in_resource->m_vertex_count );
}

} // namespace render
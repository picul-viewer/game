#include "model_mesh.h"
#include <math/math_sse.h>

#include "mesh.h"
#include "texture.h"

namespace render {

void model_mesh::destroy_resource( model_mesh* const in_resource )
{
	in_resource->m_mesh.release( );

	in_resource->m_diffuse.release( );
	in_resource->m_specular.release( );

	container( ).remove( in_resource );
}

void model_mesh::fill_gpu_data( gpu::mesh_object& in_data )
{
	in_data.index_buffer_offset = get_mesh( )->index_buffer_offset( );
	in_data.vertex_buffer_offset = get_mesh( )->vertex_buffer_offset( );
	in_data.index_count = get_mesh( )->index_count( );
	in_data.vertex_count = get_mesh( )->vertex_count( );
	in_data.diffuse_texture_index = get_diffuse_texture( )->get( );
	in_data.specular_texture_index = get_specular_texture( )->get( );
}

} // namespace render
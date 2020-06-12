#include "model_mesh.h"
#include <math/math_sse.h>

#include "mesh.h"
#include "texture.h"

namespace render {

void model_mesh::destroy_resource( model_mesh* const in_resource )
{
	in_resource->m_mesh.release( );

	in_resource->m_albedo.release( );
	in_resource->m_metal_rough.release( );

	container( ).remove( in_resource );
}

void model_mesh::fill_gpu_data( gpu::mesh_object& in_data )
{
	in_data.index_buffer_offset = get_mesh( )->index_buffer_offset( );
	in_data.vertex_buffer_offset = get_mesh( )->vertex_buffer_offset( );
	in_data.index_count = get_mesh( )->index_count( );
	in_data.vertex_count = get_mesh( )->vertex_count( );
	in_data.albedo_texture_index = get_albedo_texture( )->get( );
	in_data.metal_rough_texture_index = get_metal_rough_texture( )->get( );
}

} // namespace render
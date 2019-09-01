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

void model_mesh::render( ) const
{
	texture::from_handle( m_diffuse )->bind_ps	( 0 );
	texture::from_handle( m_specular )->bind_ps	( 1 );

	mesh::from_handle( m_mesh )->draw			( );
}

} // namespace render
#include "texture.h"
#include "resources.h"

namespace render {

void texture::destroy_resource( texture* const in_resource )
{
	g_resources.destroy_texture( in_resource->m_texture_id );
	in_resource->m_texture.destroy( );

	container( ).remove( in_resource );
}

} // namespace render
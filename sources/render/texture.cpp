#include "texture.h"
#include "texture_cook.h"

namespace render {

void texture::destroy_resource( texture* const in_resource )
{
	in_resource->m_view.destroy( );

	container( ).remove( in_resource );
}

} // namespace render
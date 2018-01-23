#include "render_object.h"

#include "render_object_mesh.h"

namespace render {

render_object::render_object( render_object_type in_type ) :
	m_type	( in_type )
{ }

void render_object::update_transform( )
{
	m_transform	= m_parent->get_transform( );
}

} // namespace render
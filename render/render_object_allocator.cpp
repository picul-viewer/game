#include "render_object_allocator.h"

namespace render {

void render_object_allocator::create( )
{
	m_allocator.create( );
}

void render_object_allocator::destroy( )
{
	m_allocator.destroy( );
}

} // namespace render
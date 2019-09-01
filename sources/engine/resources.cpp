#include "resources.h"

namespace engine {

void resources::create( )
{
	m_scene_allocator.create( nullptr, 4 * Kb );
	m_object_allocator.create( nullptr, 64 * Kb );
}

void resources::destroy( )
{
	m_scene_allocator.destroy( );
	m_object_allocator.destroy( );
}

resources g_resources;

} // namespace engine
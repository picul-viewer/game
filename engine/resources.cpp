#include "resources.h"

namespace engine {

void resources::create( )
{
	m_scene_pool.create( );
}

void resources::destroy( )
{
	m_scene_pool.destroy( );
}

resources g_resources;

} // namespace engine
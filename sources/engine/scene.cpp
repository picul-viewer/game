#include "scene.h"
#include "object.h"
#include <resource_system/api.h>

namespace engine {

void scene::destroy_resource( scene* const in_resource )
{
	resource_system::destroy_resources( resource_system::null_callback( ), &in_resource->m_render );
}

void scene::insert( object* const in_object )
{
	ASSERT( in_object != nullptr );

	ASSERT( in_object->get_scene( ) == nullptr );
	in_object->set_scene( this );

	m_render.insert( in_object->render_object( ) );
}

void scene::remove( object* const in_object )
{
	ASSERT( in_object != nullptr );
	ASSERT( in_object->get_scene( ) == this );

	m_render.remove( in_object->render_object( ) );

	in_object->set_scene( nullptr );
}

void scene::move( object* const in_object )
{
	m_render.move( in_object->render_object( ) );
}

} // namespace engine
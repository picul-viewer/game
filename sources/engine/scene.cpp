#include "scene.h"
#include "resource_system/api.h"

namespace engine {

void scene::destroy_resource( scene* const in_resource )
{
	resource_system::destroy_resources( resource_system::null_callback( ), &in_resource->m_render );
}

void scene::insert( object* const in_object )
{

}

void scene::remove( object* const in_object )
{

}

void scene::move( object* const in_object )
{

}

} // namespace engine
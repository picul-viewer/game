#include "object.h"
#include "resource_system/api.h"

#include "resources.h"

namespace engine {

void object::destroy_resource( object* const in_resource )
{
	resource_system::destroy_resources(
		resource_system::user_callback_task<on_subsystems_destroyed>( in_resource ),
		&in_resource->m_render
	);
}

void object::on_subsystems_destroyed( pointer const in_resource )
{
	g_resources.get_object_allocator( ).deallocate( in_resource );
}

void object::update( math::float4x3 const& in_transform )
{
	m_render.update( in_transform );
}

} // namespace engine
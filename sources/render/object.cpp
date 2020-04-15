#include "object.h"

#include "resources.h"

#include "render_object_mesh.h"

namespace render {

void object::destroy_resource( object* const in_resource )
{
	in_resource->for_each_mesh_object( []( render_object_mesh* const in_obj )
	{
		in_obj->destroy( );
	} );

	g_resources.render_allocator( ).deallocate( in_resource );
}

void object::update( math::float4x3 const& in_transform )
{
	for_each_mesh_object( [&in_transform]( render_object_mesh* const in_obj )
	{
		in_obj->update( in_transform );
	} );
}

uptr object::calculate_allocation_size( u32 const in_mesh_count )
{
	return sizeof(object) + sizeof(render_object_mesh) * in_mesh_count;
}

} // namespace render
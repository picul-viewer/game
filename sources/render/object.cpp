#include "object.h"

#include "resources.h"

#include "render_object_mesh.h"

namespace render {

struct functor_destroy
{
	template<typename T>
	void operator( )( T* const in_object ) const
	{ 
		UNREACHABLE_CODE
	}

	template<>
	void operator( )( render_object_mesh* const in_object ) const
	{
		in_object->destroy( );
		g_resources.get_render_object_allocator( ).mesh_allocator( ).deallocate( in_object );
	}
};

void object::destroy_resource( object* const in_resource )
{
	g_resources.get_render_object_allocator( ).for_each( in_resource->m_objects, functor_destroy( ) );
}

struct functor_update
{
	math::float4x3 const& transform;

	template<typename T>
	void operator( )( T* in_object ) const
	{
		in_object->update( transform );
	}
};

void object::update( math::float4x3 const& in_transform )
{
	g_resources.get_render_object_allocator( ).for_each( m_objects, functor_update{ in_transform } );
}

} // namespace render
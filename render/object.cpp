#include "object.h"

#include "render_resources.h"

#include "render_object_mesh.h"

namespace render {

void object::create( config& in_config )
{
	u16 const objects_count			= in_config.read<u16>( );

	for ( u16 i = 0; i < objects_count; ++i )
	{
		u8 const object_type		= in_config.read<u8>( );

		switch ( object_type )
		{
		case render_object_type_mesh:
		{
			render_object_mesh* obj	= g_resources.get_render_object_allocator( ).allocate<render_object_mesh>( );
			obj->create				( in_config );
			m_objects.insert		( obj );
		}
		default:
			UNREACHABLE_CODE;
		}
	}
}

struct destroy_render_object
{
	template<typename T>
	void call( T* in_object )
	{
		in_object->destroy( );
	}
};

void object::destroy( )
{
	m_objects.for_each( []( render_object* current )
	{
		g_resources.get_render_object_allocator( ).execute( current, destroy_render_object( ) );
	} );
}

} // namespace render
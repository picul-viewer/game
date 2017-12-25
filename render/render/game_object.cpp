#include "game_object.h"

#include "renderer.h"

#include "render_allocators.h"

#include "render_object_mesh.h"

namespace render {

namespace renderer {
	extern render_scene				g_scene;
	extern render_objects_allocator	g_render_objects_allocator;
}

void game_object::create( config& in_config )
{
	u16 const objects_count			= in_config.read<u16>( );

	for ( u16 i = 0; i < objects_count; ++i )
	{
		u8 const object_type		= in_config.read<u8>( );

		switch ( object_type )
		{
		case render_object_type_mesh:
		{
			render_object_mesh* obj	= renderer::g_render_objects_allocator.allocate<render_object_mesh>( );
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
	void call( render_object* in_object )
	{
		in_object->destroy( );
	}
};

void game_object::destroy( )
{
	m_objects.for_each( []( render_object* current )
	{
		renderer::g_render_objects_allocator.execute_typed( current, destroy_render_object( ) );
	} );
}

} // namespace render
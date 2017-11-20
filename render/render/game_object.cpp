#include "game_object.h"

#include "renderer.h"

#include "render_object_mesh.h"

namespace render {

namespace renderer {
	extern scene g_scene;
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
			render_object_mesh* obj	= renderer::g_scene.insert_render_object_mesh( );
			obj->create				( in_config );
			m_objects.insert		( obj );
		}
		default:
			UNREACHABLE_CODE;
		}
	}
}

void game_object::destroy( )
{
	m_objects.for_each( []( render_object* current )
	{
		switch ( current->get_type( ) )
		{
		case render_object_type_mesh:
		{
			render_object_mesh* obj = (render_object_mesh*)current;
			renderer::g_scene.remove_render_object_mesh( obj );
			obj->destroy( );
			return;
		}
		default:
			UNREACHABLE_CODE;
		}
	} );
}

void game_object::dispatch( ) const
{
	m_objects.for_each( []( render_object const* obj ) { obj->dispatch( ); } );
}

} // namespace render
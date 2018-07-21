#ifndef __engine_scene_h_included_
#define __engine_scene_h_included_

#include <types.h>

namespace render { class scene; }

namespace engine {

class object;

class scene
{
public:
	void create( bool in_create_render );
	void destroy( );

	void remove_all_static_objects( );
	void insert_static_object( object* in_object );
	void build_static_scene( );

	void make_current( ) const;

	inline render::scene* get_render( ) { return m_render; }

protected:
	render::scene* m_render;

};

} // namespace engine

#endif // #ifndef __engine_scene_h_included_
#ifndef GUARD_ENGINE_SCENE_H_INCLUDED
#define GUARD_ENGINE_SCENE_H_INCLUDED

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

#endif // #ifndef GUARD_ENGINE_SCENE_H_INCLUDED
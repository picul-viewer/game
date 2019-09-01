#ifndef GUARD_ENGINE_SCENE_H_INCLUDED
#define GUARD_ENGINE_SCENE_H_INCLUDED

#include <types.h>

#include <resource_system/default_resource.h>
#include <utils/engine_threads.h>

#include <render/scene.h>

namespace engine {

class object;

class scene : public default_resource<scene>
{
public:
	friend class scene_cook;

public:
	enum : u32 {
		destroy_thread_index = engine_thread_count
	};

	static void destroy_resource( scene* const in_resource );

public:
	void insert( object* const in_object );
	void remove( object* const in_object );
	void move( object* const in_object );

	inline render::scene& render_scene( ) { return m_render; }

private:
	render::scene m_render;

};

} // namespace engine

#endif // #ifndef GUARD_ENGINE_SCENE_H_INCLUDED
#ifndef GUARD_ENGINE_RESOURCES_H_INCLUDED
#define GUARD_ENGINE_RESOURCES_H_INCLUDED

#include <types.h>
#include <lib/pool_allocator.h>

#include "scene.h"
#include "object.h"

namespace engine {

class resources
{
public:
	typedef lib::pool_allocator<sizeof(scene)> scene_allocator_type;
	typedef lib::pool_allocator<sizeof(object)> object_allocator_type;

public:
	void create( );
	void destroy( );

	inline scene_allocator_type& get_scene_allocator( ) { return m_scene_allocator; }
	inline scene_allocator_type& get_object_allocator( ) { return m_scene_allocator; }

private:
	scene_allocator_type m_scene_allocator;
	object_allocator_type m_object_allocator;

};

extern resources g_resources;

} // namespace engine

#endif // #ifndef GUARD_ENGINE_RESOURCES_H_INCLUDED
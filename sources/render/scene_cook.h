#ifndef GUARD_RENDER_SCENE_COOK_H_INCLUDED
#define GUARD_RENDER_SCENE_COOK_H_INCLUDED

#include <types.h>

#include <lib/reader.h>
#include <math/bvh.h>

#include <resource_system/queried_resources.h>
#include <resource_system/default_resource_cook.h>
#include <utils/engine_threads.h>

namespace render {

class scene;

class scene_cook : public default_resource_cook<scene, scene_cook>
{
public:
	static scene_cook* create( scene* const in_resource_ptr, lib::reader const& in_config );
	static void destroy( pointer const in_cook );

public:
	enum : u32 {
		create_thread_index = engine_helper_thread_0
	};

	void create_resource( );

private:
	void on_child_resources_ready( queried_resources& in_queried );
	void on_copy_tasks_finished( );

private:
	scene* m_result;
	lib::reader m_config;
	u32 m_static_mesh_count;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_SCENE_COOK_H_INCLUDED
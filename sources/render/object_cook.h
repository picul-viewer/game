#ifndef GUARD_RENDER_OBJECT_COOK_H_INCLUDED
#define GUARD_RENDER_OBJECT_COOK_H_INCLUDED

#include <types.h>

#include <lib/reader.h>

#include <resource_system/queried_resources.h>
#include <resource_system/default_resource_cook.h>
#include <utils/engine_threads.h>

namespace render {

class object;
class scene;

class object_cook : public default_resource_cook<object, object_cook>
{
public:
	object_cook( lib::reader const& in_config );

public:
	enum : u32 {
		create_thread_index = engine_helper_thread_0
	};

	void create_resource( );

private:
	void on_child_resources_ready( queried_resources& in_queried );
	void on_copy_tasks_finished( );

private:
	object* m_result;
	lib::reader m_config;

};

} // namespace render

#endif // #ifndef GUARD_RENDER_OBJECT_COOK_H_INCLUDED
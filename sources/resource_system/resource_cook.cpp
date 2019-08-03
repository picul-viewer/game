#include "resource_cook.h"
#include "resource_system.h"

namespace resource_system {

void resource_cook::init( query_functor const in_destroyer )
{
	m_destroyer = in_destroyer;
}

void resource_cook::create_child_resources(
	query_info const* const in_queries,
	u32 const in_query_count,
	cook_task_info const& in_callback_info
)
{
	query_info callback;
	callback.this_ptr = this;
	callback.task = in_callback_info;
	g_resource_system.create_child_resources( in_queries, in_query_count, callback );
}

void resource_cook::execute_tasks(
	cook_task_info const* const in_tasks,
	u32 const in_task_count,
	cook_task_info const& in_callback
)
{
	g_resource_system.execute_tasks( this, in_tasks, in_task_count, in_callback );
}

void resource_cook::finish( pvoid  const in_result )
{
	g_resource_system.finish_cook( this, (query_result)in_result );
}

} // namespace resource_system
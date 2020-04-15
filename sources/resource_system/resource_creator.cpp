#include "resource_creator.h"
#include "resource_system.h"

namespace resource_system {

void resource_creator::execute_child_tasks(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback_info
)
{
	g_resource_system.create_subtasks(
		&in_callback_info,
		in_tasks,
		in_task_count,
		false
	);
}

void resource_creator::create_child_resources(
	task_info const* const in_tasks,
	u32 const in_task_count,
	task_info const& in_callback_info
)
{
	g_resource_system.create_subtasks(
		&in_callback_info,
		in_tasks,
		in_task_count,
		true
	);
}

custom_task_context resource_creator::create_child_custom_tasks(
	u32 const in_task_count,
	task_info const& in_callback_info
)
{
	return g_resource_system.create_custom_subtasks(
		in_task_count,
		&in_callback_info
	);
}

} // namespace resource_system
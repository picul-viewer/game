#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_CREATOR_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_CREATOR_H_INCLUDED

#include <types.h>
#include <macros.h>
#include <lib/allocator.h>
#include "resource_system_types.h"
#include "resource_system_utils.h"

namespace resource_system {

class resource_creator
{
protected:
	void execute_child_tasks(
		task_info const* const in_tasks,
		u32 const in_task_count,
		task_info const& in_callback_info
	);

	void create_child_resources(
		task_info const* const in_tasks,
		u32 const in_task_count,
		task_info const& in_callback_info
	);

	template<typename ... Cooks>
	inline void create_child_resources(
		task_info const& in_callback_info,
		Cooks* ... in_cooks
	);

	custom_task_context create_child_custom_tasks(
		u32 const in_task_count,
		task_info const& in_callback_info
	);

};

template<typename ... Cooks>
void resource_creator::create_child_resources( task_info const& in_callback_info, Cooks* ... in_cooks )
{
	u32 const count = type_count<Cooks ...>::value;
	task_info* const tasks = stack_allocate( count * sizeof(task_info) );

	gather_task_info( gather_create_resource_task_info( ), tasks, in_cooks ... );

	create_child_resources( tasks, count, in_callback_info );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_CREATOR_H_INCLUDED
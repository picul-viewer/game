#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED

#include <types.h>
#include <macros.h>
#include <lib/allocator.h>
#include "resource_system_types.h"
#include "resource_system_utils.h"

namespace resource_system {

class resource_cook
{
protected:
	typedef void ( *cook_destroyer )( pointer );

protected:
	void init( cook_destroyer const in_destroyer );

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

	void finish_cook( pvoid const in_result );
	void finish_cook( pvoid const in_result, u32 const in_task_offset, u32 const in_result_index );

	void get_result_links( u32& in_parent_data_offset, u32& in_result_index ) const;

private:
	cook_destroyer m_destroyer;

};

template<typename ... Cooks>
void resource_cook::create_child_resources( task_info const& in_callback_info, Cooks* ... in_cooks )
{
	u32 const count = type_count<Cooks ...>::value;
	task_info* const tasks = stack_allocate( count * sizeof(task_info) );

	gather_task_info( gather_create_resource_task_info( ), tasks, in_cooks ... );

	create_child_resources( tasks, count, in_callback_info );
}

} // namespace resource_system

using resource_system::resource_cook;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_COOK_H_INCLUDED
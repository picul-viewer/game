#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED

#include <types.h>

namespace resource_system {

typedef void( *task_functor )( pvoid* const, u32 const, pointer const );

struct task_info
{
	task_functor functor;
	pointer user_data;
	u32 thread_index;
};

typedef pvoid custom_task_context;

} // namespace resource_system

using resource_system::task_info;
using resource_system::custom_task_context;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED
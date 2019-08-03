#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED

#include <types.h>

namespace resource_system {

class queried_resources;

typedef void( *user_callback )( queried_resources&, pointer const, uptr const );
typedef void( *cook_functor )( pointer const, queried_resources& );
typedef void( *query_functor )( pointer const );

struct cook_task_info
{
	cook_functor functor;
	u32 thread_index;
};

struct query_info
{
	pointer this_ptr;
	cook_task_info task;
};

} // namespace resource_system

using resource_system::cook_task_info;
using resource_system::query_info;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_TYPES_H_INCLUDED
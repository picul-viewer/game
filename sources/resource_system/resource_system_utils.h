#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_UTILS_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_UTILS_H_INCLUDED

#include <types.h>
#include <macros.h>
#include "resource_system_types.h"

namespace resource_system {

template<typename GatherFunctor, typename ... Args>
inline void gather_task_info( GatherFunctor const& in_functor, task_info* const in_info, Args* ... in_args );

template<typename GatherFunctor, typename Arg, typename ... Args>
inline void gather_task_info( GatherFunctor const& in_functor, task_info* const in_info, Arg* in_arg, Args const& ... in_args )
{
	ASSERT( in_arg != nullptr );

	in_functor( in_arg, in_info[0] );

	gather_task_info( in_functor, in_info + 1, in_args ... );
}

template<typename GatherFunctor>
inline void gather_task_info( GatherFunctor const& in_functor, task_info* const in_info )
{ }

struct gather_create_resource_task_info
{
	template<typename CookType>
	void operator( )( CookType* const in_cook, task_info& in_info ) const
	{
		in_cook->fill_task_info( in_info );
	}
};

struct gather_destroy_resource_task_info
{
	template<typename ResourceType>
	void operator( )( ResourceType* const in_cook, task_info& in_info ) const
	{
		in_cook->fill_destroy_task_info( in_info );
	}
};

} // namespace resource_system

using resource_system::task_info;

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_UTILS_H_INCLUDED
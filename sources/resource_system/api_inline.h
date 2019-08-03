#ifndef GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED

#include <lib/allocator.h>
#include "resource_cook.h"

namespace resource_system {

template<typename ... Cooks>
void create_resources(
	user_callback const& in_callback,
	pointer const in_callback_data,
	uptr const in_callback_data_size,
	Cooks* ... in_cooks
)
{
	u32 const count = type_count<Cooks ...>::value;
	query_info* const queries = stack_allocate( count * sizeof(query_info) );

	fill_query_info( queries, in_cooks ... );

	create_resources( queries, count, in_callback, in_callback_data, in_callback_data_size );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED
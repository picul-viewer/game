#ifndef GUARD_RESOURCE_SYSTEM_CALLBACK_ALLOCATOR_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_CALLBACK_ALLOCATOR_H_INCLUDED

#include <types.h>
#include <lib/temporal_allocator.h>
#include "resource_system_types.h"

namespace resource_system {

class callback_allocator
{
public:
	void create( );
	void destroy( );

	query_callback_id register_callback( query_callback const& in_functor, pointer const in_data, uptr const in_data_size );
	void invoke_callback( query_callback_id const in_callback_id, queried_resources& in_queried_resources );

private:
	temporal_allocator<4> m_allocator;

};

extern callback_allocator g_callback_allocator;

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_CALLBACK_ALLOCATOR_H_INCLUDED
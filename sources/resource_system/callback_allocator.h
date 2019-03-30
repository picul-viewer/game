#ifndef __resource_system_callback_allocator_h_included_
#define __resource_system_callback_allocator_h_included_

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

#endif // #ifndef __resource_system_callback_allocator_h_included_
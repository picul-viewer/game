#ifndef __render_resource_pool_h_included_
#define __render_resource_pool_h_included_

#include <types.h>
#include <lib/weak_string.h>
#include <lib/hash_map.h>

namespace render {

template<typename Resource>
class resource_pool
{
public:
	resource_pool( );

	void create( uptr in_hash_map_table_size );
	void destroy( );

	Resource* get( weak_const_string const& in_filename );

protected:
	typedef object_hash_map32<weak_const_string, Resource> pool_data;
	typedef dynamic_pool<pool_data::kv_pool_element_size, 4 * Kb, 256> pool_data_allocator;
	typedef dynamic_allocation_multipool<Memory_Page_Size, 256> string_pool;

	string_pool			m_string_pool;
	pool_data_allocator	m_pool_data_allocator;
	pool_data			m_pool_data;
};

} // namespace render

#include "resource_pool_inline.h"

#endif // #ifndef __render_resource_pool_h_included_
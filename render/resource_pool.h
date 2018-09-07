#ifndef __render_resource_pool_h_included_
#define __render_resource_pool_h_included_

#include <types.h>
#include <lib/weak_string.h>
#include <lib/pool.h>
#include "resource_path_registry.h"

namespace render {

template<typename Resource>
class resource_pool
{
public:

public:
	resource_pool( );

	void create( resource_path_registry& in_resource_path_registry );
	void destroy( );
	
	Resource* create_resource( );
	void destroy_resource( Resource* in_resource );
	Resource* get_resource( weak_const_string const& in_path );

protected:
	static void load_resource( Resource* out_resource, weak_const_string const& in_path );

protected:
	typedef dynamic_pool<sizeof(Resource), Memory_Page_Size, 256> resource_allocator;

	resource_allocator		m_resource_allocator;
	resource_path_registry*	m_path_registry;
};

} // namespace render

#include "resource_pool_inline.h"

#endif // #ifndef __render_resource_pool_h_included_
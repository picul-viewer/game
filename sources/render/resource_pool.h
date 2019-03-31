#ifndef GUARD_RENDER_RESOURCE_POOL_H_INCLUDED
#define GUARD_RENDER_RESOURCE_POOL_H_INCLUDED

#include <types.h>
#include <lib/weak_string.h>
#include <lib/pool.h>
#include "resource_path_registry.h"

namespace render {

template<typename Resource>
class resource_pool
{
public:
	resource_pool( );

	void create( resource_path_registry& in_resource_path_registry );
	void destroy( );
	
	Resource* create_resource( );
	Resource* load_resource( weak_const_string const& in_path );

	void free_resource( Resource* const in_resource );
	
	Resource* operator[]( u32 const in_handle );
	uptr index_of( Resource* const in_resource );

private:
	static void create_resource_from_file( Resource* const in_resource, weak_const_string const& in_path );

protected:
	typedef dynamic_pool<sizeof(Resource), Memory_Page_Size, 256> resource_allocator;

	resource_allocator		m_resource_allocator;
	resource_path_registry*	m_path_registry;
};

} // namespace render

#include "resource_pool_inline.h"

#endif // #ifndef GUARD_RENDER_RESOURCE_POOL_H_INCLUDED
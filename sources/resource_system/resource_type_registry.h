#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_H_INCLUDED

#include <types.h>
#include <utils/resource_types.h>
#include "resource_system_types.h"

namespace resource_system {

class resource_type_registry
{
public:
	template<typename ResourceType>
	inline void register_resource_type( );
	
private:
	friend class resource_system;

	struct resource_type_data
	{
		cook_functor executor;
		u32 executor_thread_index;
	};

	resource_type_data& operator[]( resource_type const in_resource_type );

private:
	resource_type_data m_data[resource_type_count];
};

extern resource_type_registry g_resource_type_registry;

} // namespace resource_system

#include "resource_type_registry_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_TYPE_REGISTRY_H_INCLUDED
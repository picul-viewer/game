#ifndef GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED

#include "resource_type_registry.h"

namespace resource_system {

template<typename ResourceType>
void register_resource_type( )
{
	g_resource_type_registry.register_resource_type<ResourceType>( );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_API_INLINE_H_INCLUDED
#ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_INLINE_H_INCLUDED

#include <macros.h>
#include <lib/algorithms.h>

namespace resource_system {

template<typename ResourceType>
typename ResourceType::handle_type queried_resources::get_resource( )
{
	ASSERT( m_index < m_count );
	return lib::move( typename ResourceType::handle_type( m_results[m_index++] ) );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_INLINE_H_INCLUDED
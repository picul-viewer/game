#ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_INLINE_H_INCLUDED

#include <macros.h>

namespace resource_system {

template<typename ResourceType>
resource_ptr<ResourceType>&& queried_resources::get_resource( )
{
	ASSERT( m_index < m_count );
	return std::move( resource_ptr<ResourceType>( m_results[m_index++] ) );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_INLINE_H_INCLUDED
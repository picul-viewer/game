#ifndef __resource_system_queried_resources_inline_h_included_
#define __resource_system_queried_resources_inline_h_included_

#include <macros.h>

namespace resource_system {

template<typename ResourceType>
resource_ptr<ResourceType>&& queried_resources::get_resource( )
{
	ASSERT( m_index < m_count );
	return std::move( resource_ptr<ResourceType>( m_results[m_index++] ) );
}

} // namespace resource_system

#endif // #ifndef __resource_system_queried_resources_inline_h_included_
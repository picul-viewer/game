#ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_H_INCLUDED

#include <types.h>

namespace resource_system {

class queried_resources
{
public:
	queried_resources( );
	queried_resources( pvoid* const in_results, u32 const in_results_count );

	queried_resources( queried_resources const& ) = delete;
	queried_resources& operator=( queried_resources const& ) = delete;

	queried_resources( queried_resources&& in_other );
	queried_resources& operator=( queried_resources&& in_other );

	~queried_resources( );

	template<typename ResourceDescriptor>
	typename ResourceDescriptor get_resource( );

	uptr index( ) const;
	uptr count( ) const;

private:
	pvoid* m_results;
	u32 m_index;
	u32 m_count;

};

} // namespace resource_system

using resource_system::queried_resources;

#include "queried_resources_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_H_INCLUDED
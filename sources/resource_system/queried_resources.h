#ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"
#include "file_ptr.h"
#include "request_ptr.h"
#include "resource_ptr.h"

namespace resource_system {

class queried_resources
{
public:
	queried_resources( ) = delete;

	queried_resources( queried_resources const& ) = delete;
	queried_resources& operator=( queried_resources const& ) = delete;
	queried_resources( queried_resources&& ) = delete;
	queried_resources& operator=( queried_resources&& ) = delete;

	~queried_resources( );

	file_ptr&& get_file( );
	
	request_ptr&& get_request( );

	template<typename ResourceType>
	resource_ptr<ResourceType>&& get_resource( );

private:
	friend class resource_system;
	queried_resources( query_result* const in_results, u32 const in_results_count );

private:
	query_result* m_results;
	u32 m_index;
	u32 m_count;

};

} // namespace resource_system

#include "queried_resources_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_QUERIED_RESOURCES_H_INCLUDED
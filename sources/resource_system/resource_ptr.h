#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_PTR_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_PTR_H_INCLUDED

#include <types.h>
#include "resource_system_types.h"

namespace resource_system {

template<typename ResourceType>
class resource_ptr
{
public:
	resource_ptr( ) = default;
	
	resource_ptr( resource_ptr const& in_other );
	resource_ptr& operator=( resource_ptr const& in_other );

	resource_ptr( resource_ptr&& in_other );
	resource_ptr& operator=( resource_ptr&& in_other );

	~resource_ptr( );

	ResourceType* get( ) const;

	ResourceType* operator->( ) const;

private:
	friend class queried_resources;
	resource_ptr( query_result const in_query_result );

private:
	ResourceType* m_resource = nullptr;

};

} // namespace resource_system

#include "resource_ptr_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_PTR_H_INCLUDED
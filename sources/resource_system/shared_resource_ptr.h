#ifndef GUARD_RESOURCE_SHARED_SYSTEM_RESOURCE_PTR_H_INCLUDED
#define GUARD_RESOURCE_SHARED_SYSTEM_RESOURCE_PTR_H_INCLUDED

#include <types.h>

namespace resource_system {

template<typename ResourceType>
class shared_resource_ptr
{
public:
	typedef typename ResourceType::handle_type resource_handle_type;

public:
	shared_resource_ptr( ) = default;
	
	shared_resource_ptr( resource_handle_type&& in_handle );

	shared_resource_ptr( shared_resource_ptr const& in_other );
	shared_resource_ptr& operator=( shared_resource_ptr const& in_other );

	shared_resource_ptr( shared_resource_ptr&& in_other );
	shared_resource_ptr& operator=( shared_resource_ptr&& in_other );

	~shared_resource_ptr( );

	ResourceType* get( ) const;
	operator ResourceType*( ) const;

	ResourceType* operator->( ) const;

	void reset( );

private:
	friend class queried_resources;
	shared_resource_ptr( pvoid const in_ptr );

private:
	ResourceType* m_resource = nullptr;

};

} // namespace resource_system

using resource_system::shared_resource_ptr;

#include "shared_resource_ptr_inline.h"

#endif // #ifndef GUARD_RESOURCE_SHARED_SYSTEM_RESOURCE_PTR_H_INCLUDED
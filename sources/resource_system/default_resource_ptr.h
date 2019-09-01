#ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_PTR_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_PTR_H_INCLUDED

#include <types.h>

namespace resource_system {

template<typename ResourceType>
class default_resource_ptr
{
public:
	default_resource_ptr( ) = default;

	default_resource_ptr( default_resource_ptr const& ) = delete;
	default_resource_ptr& operator=( default_resource_ptr const& ) = delete;

	default_resource_ptr( default_resource_ptr&& in_other );
	default_resource_ptr& operator=( default_resource_ptr&& in_other );

	~default_resource_ptr( );

	ResourceType* get( ) const;
	operator ResourceType*( ) const;

	ResourceType* operator->( ) const;

	void reset( );

private:
	friend class queried_resources;
	default_resource_ptr( pvoid const in_ptr );

private:
	ResourceType* m_resource = nullptr;

};

} // namespace resource_system

using resource_system::default_resource_ptr;

#include "default_resource_ptr_inline.h"

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_PTR_H_INCLUDED
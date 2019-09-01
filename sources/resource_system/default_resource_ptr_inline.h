#ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_PTR_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_PTR_INLINE_H_INCLUDED

#include <lib/algorithms.h>

namespace resource_system {

template<typename ResourceType>
default_resource_ptr<ResourceType>::default_resource_ptr( pvoid const in_ptr ) :
	m_resource( (ResourceType*)in_ptr )
{ }

template<typename ResourceType>
default_resource_ptr<ResourceType>::default_resource_ptr( default_resource_ptr<ResourceType>&& in_other )
{
	m_resource = in_other.m_resource;
	in_other.m_resource = nullptr;
}

template<typename ResourceType>
default_resource_ptr<ResourceType>& default_resource_ptr<ResourceType>::operator=( default_resource_ptr<ResourceType>&& in_other )
{
	lib::swap( m_resource, in_other.m_resource );
	return *this;
}

template<typename ResourceType>
default_resource_ptr<ResourceType>::~default_resource_ptr( )
{
	if ( m_resource )
	{
		m_resource->destroy( );
	}
}

template<typename ResourceType>
ResourceType* default_resource_ptr<ResourceType>::get( ) const
{
	return m_resource;
}

template<typename ResourceType>
default_resource_ptr<ResourceType>::operator ResourceType*( ) const
{
	return m_resource;
}

template<typename ResourceType>
ResourceType* default_resource_ptr<ResourceType>::operator->( ) const
{
	return m_resource;
}

template<typename ResourceType>
void default_resource_ptr<ResourceType>::reset( )
{
	m_resource = nullptr;
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_DEFAULT_RESOURCE_PTR_INLINE_H_INCLUDED
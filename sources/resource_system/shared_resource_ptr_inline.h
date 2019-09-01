#ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_PTR_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_PTR_INLINE_H_INCLUDED

#include <lib/algorithms.h>

namespace resource_system {

template<typename ResourceType>
shared_resource_ptr<ResourceType>::shared_resource_ptr( pvoid const in_ptr ) :
	m_resource( (ResourceType*)in_ptr )
{ }

template<typename ResourceType>
shared_resource_ptr<ResourceType>::shared_resource_ptr( resource_handle_type&& in_handle ) :
	m_resource( ( in_handle == resource_handle_type::invalid ) ? nullptr : ResourceType::from_handle( in_handle ) )
{
	in_handle.m_id = resource_handle_type::invalid;
}

template<typename ResourceType>
shared_resource_ptr<ResourceType>::shared_resource_ptr( shared_resource_ptr<ResourceType> const& in_other ) :
	m_resource( in_other.m_resource )
{
	if ( m_resource )
		m_resource->add_ref( );
}

template<typename ResourceType>
shared_resource_ptr<ResourceType>& shared_resource_ptr<ResourceType>::operator=( shared_resource_ptr<ResourceType> const& in_other )
{
	if ( m_resource )
		m_resource->release( );

	m_resource = in_other.m_resource;

	if ( m_resource )
		m_resource->add_ref( );

	return *this;
}

template<typename ResourceType>
shared_resource_ptr<ResourceType>::shared_resource_ptr( shared_resource_ptr<ResourceType>&& in_other )
{
	m_resource = in_other.m_resource;
	in_other.m_resource = nullptr;
}

template<typename ResourceType>
shared_resource_ptr<ResourceType>& shared_resource_ptr<ResourceType>::operator=( shared_resource_ptr<ResourceType>&& in_other )
{
	lib::swap( m_resource, in_other.m_resource );
	return *this;
}

template<typename ResourceType>
shared_resource_ptr<ResourceType>::~shared_resource_ptr( )
{
	if ( m_resource )
		m_resource->release( );
}

template<typename ResourceType>
ResourceType* shared_resource_ptr<ResourceType>::get( ) const
{
	return m_resource;
}

template<typename ResourceType>
shared_resource_ptr<ResourceType>::operator ResourceType*( ) const
{
	return m_resource;
}

template<typename ResourceType>
ResourceType* shared_resource_ptr<ResourceType>::operator->( ) const
{
	return m_resource;
}

template<typename ResourceType>
void shared_resource_ptr<ResourceType>::reset( )
{
	m_resource = nullptr;
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_PTR_INLINE_H_INCLUDED
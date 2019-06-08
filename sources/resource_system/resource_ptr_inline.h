#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_PTR_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_PTR_INLINE_H_INCLUDED

#include <lib/algorithms.h>

namespace resource_system {

template<typename ResourceType>
resource_ptr<ResourceType>::resource_ptr( query_result const in_query_result ) :
	m_resource( (ResourceType*)in_query_result )
{ }

template<typename ResourceType>
resource_ptr<ResourceType>::resource_ptr( resource_ptr<ResourceType> const& in_other ) :
	m_resource( in_other.m_resource )
{
	if ( m_resource )
		m_resource->add_ref( );
}

template<typename ResourceType>
resource_ptr<ResourceType>& resource_ptr<ResourceType>::operator=( resource_ptr<ResourceType> const& in_other )
{
	if ( m_resource )
		m_resource->release( );

	m_resource = in_other.m_resource;

	if ( m_resource )
		m_resource->add_ref( );

	return *this;
}

template<typename ResourceType>
resource_ptr<ResourceType>::resource_ptr( resource_ptr<ResourceType>&& in_other )
{
	m_resource = in_other.m_resource;
	in_other.m_resource = nullptr;
}

template<typename ResourceType>
resource_ptr<ResourceType>& resource_ptr<ResourceType>::operator=( resource_ptr<ResourceType>&& in_other )
{
	lib::swap( m_resource, in_other.m_resource );
	return *this;
}

template<typename ResourceType>
resource_ptr<ResourceType>::~resource_ptr( )
{
	if ( m_resource )
		m_resource->release( );
}

template<typename ResourceType>
ResourceType* resource_ptr<ResourceType>::get( ) const
{
	return m_resource;
}

template<typename ResourceType>
ResourceType* resource_ptr<ResourceType>::operator->( ) const
{
	return m_resource;
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_PTR_INLINE_H_INCLUDED
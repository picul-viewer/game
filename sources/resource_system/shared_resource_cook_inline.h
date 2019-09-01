#ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_COOK_INLINE_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_COOK_INLINE_H_INCLUDED

namespace resource_system {

template<typename Resource, typename ResourceCook>
void
shared_resource_cook<Resource, ResourceCook>::
init( shared_resource_id const in_id )
{
	default_resource_cook<Resource, ResourceCook>::init( );
	m_id = in_id;
	m_next_subscriber = nullptr;
	m_initial_ref_count = 1;
}

template<typename Resource, typename ResourceCook>
void
shared_resource_cook<Resource, ResourceCook>::
check_resource_exists(
	void ( ResourceCook::*in_callback )( Resource* const )
)
{
	get_result_links( m_parent_task_offset, m_result_index );

	Resource* resource;
	bool const resource_exists = Resource::container( ).get( m_id, this, resource );

	if ( resource_exists )
	{
		// Here there is a little chance, that this code will be executed when 
		// cook object will be already disposed. In this case immediate exit takes
		// place, and no load from cook object occur, so this should be totally safe.
		if ( resource == nullptr )
			return;

		finish( resource );
	}

	( ( (ResourceCook*)this )->*in_callback )( resource );
}

template<typename Resource, typename ResourceCook>
void
shared_resource_cook<Resource, ResourceCook>::
finish( Resource* const in_result )
{
	( (Resource*)in_result )->finish_creation( m_id, m_initial_ref_count );

	finish_impl( in_result );
}

template<typename Resource, typename ResourceCook>
shared_resource_id
shared_resource_cook<Resource, ResourceCook>::
id( ) const
{
	return m_id;
}

template<typename Resource, typename ResourceCook>
void
shared_resource_cook<Resource, ResourceCook>::
add_subscriber( this_type* const in_cook )
{
	in_cook->m_next_subscriber = m_next_subscriber;
	m_next_subscriber = in_cook;
	++m_initial_ref_count;
}

template<typename Resource, typename ResourceCook>
void
shared_resource_cook<Resource, ResourceCook>::
finish_impl( pvoid const in_result )
{
	if ( m_next_subscriber )
		m_next_subscriber->finish_impl( in_result );

	finish_cook( in_result, m_parent_task_offset, m_result_index );
}

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_SHARED_RESOURCE_COOK_INLINE_H_INCLUDED
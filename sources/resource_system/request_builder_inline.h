#ifndef __resource_system_request_builder_inline_h_included_
#define __resource_system_request_builder_inline_h_included_

namespace resource_system {

template<typename CookDataType>
query<CookDataType>::query( request_builder& in_builder, CookDataType* const in_cook_data ) :
	query_base( in_builder, CookDataType::resource_type, in_cook_data, in_cook_data->calculate_size( ) )
{ }

template<typename CookDataType>
CookDataType* query<CookDataType>::operator->( )
{
	return m_cook_data;
}

template<typename CookDataType>
CookDataType const* query<CookDataType>::operator->( ) const
{
	return m_cook_data;
}


template<typename CookDataType>
query<CookDataType>& request_builder::create_query( CookDataType* const in_cook_data )
{
	query<CookDataType>* const result = m_allocator.allocate( sizeof(query<CookDataType>) );
	new ( result ) query<CookDataType>( *this, in_cook_data );
	
	m_resources.insert( result );
	++m_resources_count;

	return *result;
}

} // namespace resource_system

#endif // #ifndef __resource_system_request_builder_inline_h_included_
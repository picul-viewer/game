#include "queried_resources.h"
#include <macros.h>

namespace resource_system {

queried_resources::queried_resources( query_result* const in_results, u32 const in_results_count ) :
	m_results( in_results ),
	m_index( 0 ),
	m_count( in_results_count )
{ }

queried_resources::~queried_resources( )
{
	ASSERT( m_index == m_count );
}

file_ptr&& queried_resources::get_file( )
{
	ASSERT( m_index < m_count );
	return std::move( file_ptr( m_results[m_index++] ) );
}

request_ptr&& queried_resources::get_request( )
{
	ASSERT( m_index < m_count );
	return std::move( request_ptr( m_results[m_index++] ) );
}

} // namespace resource_system
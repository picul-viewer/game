#include "queried_resources.h"
#include <macros.h>

namespace resource_system {

queried_resources::queried_resources( ) :
	m_results( nullptr ),
	m_index( 0 ),
	m_count( 0 )
{ }

queried_resources::queried_resources( pvoid* const in_results, u32 const in_results_count ) :
	m_results( in_results ),
	m_index( 0 ),
	m_count( in_results_count )
{ }

queried_resources::queried_resources( queried_resources&& in_other ) :
	m_results( in_other.m_results ),
	m_index( in_other.m_index ),
	m_count( in_other.m_count )
{
	in_other.m_results = nullptr;
	in_other.m_index = 0;
	in_other.m_count = 0;
}

queried_resources& queried_resources::operator=( queried_resources&& in_other )
{
	lib::swap( m_results, in_other.m_results );
	lib::swap( m_index, in_other.m_index );
	lib::swap( m_count, in_other.m_count );

	return *this;
}

queried_resources::~queried_resources( )
{
	ASSERT_CMP( m_index, ==, m_count );
}

uptr queried_resources::index( ) const
{
	return m_index;
}

uptr queried_resources::count( ) const
{
	return m_count;
}

} // namespace resource_system
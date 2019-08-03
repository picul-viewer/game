#include "linear_allocator.h"

void linear_allocator::create( pointer const memory, uptr const size )
{
	m_data		= memory;
	m_data_end	= memory + size;
	clear		( );
}

void linear_allocator::destroy( )
{ }

pointer linear_allocator::data( ) const
{
	return m_data;
}

pointer linear_allocator::data_end( ) const
{
	return m_last_pointer;
}

void linear_allocator::clear( )
{
	m_last_pointer = m_data;
}

pointer linear_allocator::allocate( uptr size )
{
	ASSERT_CMP				( m_last_pointer + size, <=, m_data_end );
	pointer const result	= m_last_pointer;
	m_last_pointer			+= size;
	return result;
}

void linear_allocator::shrink( uptr const shrink_size )
{
	ASSERT_CMP				( (uptr)( m_last_pointer - m_data ), >=, shrink_size );
	m_last_pointer			-= shrink_size;
}

uptr linear_allocator::index_of( pointer const p ) const
{
	ASSERT_CMP( p, >=, m_data );
	ASSERT_CMP( p, <=, m_last_pointer );

	uptr const result = p - m_data;

	return result;
}
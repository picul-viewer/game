#include "config.h"
#include <lib/memory.h>
#include <lib/hash.h>

config::config( pointer data, uptr size ) :
	m_data		( data ),
	m_size		( size ),
	m_pointer	( data )
{ }

void config::create( pointer data, uptr size )
{
	m_data		= data;
	m_size		= size;
	m_pointer	= data;
}

void config::operator+=( ptr size )
{
	m_pointer += size;
	ASSERT( ( m_pointer >= m_data ) && ( m_pointer < m_data + size ) );
}

void config::reset( )
{
	m_pointer = m_data;
}

pointer config::data( ) const
{
	return m_data;
}

pointer config::current( ) const
{
	return m_pointer;
}

uptr config::size( ) const
{
	return m_pointer - m_data;
}

uptr config::capacity( ) const
{
	return m_size;
}

u32 config::hash( ) const
{
	return hash32( )( data( ), size( ) );
}


bool operator==( config const& l, config const& r )
{
	if ( l.size( ) != r.size( ) )
		return false;
	return memory::equal( l.data( ), r.data( ), l.size( ) );
}

bool operator!=( config const& l, config const& r )
{
	if ( l.size( ) != r.size( ) )
		return true;
	return !memory::equal( l.data( ), r.data( ), l.size( ) );
}

bool operator<( config const& l, config const& r )
{
	// Not lexic order, but it's faster.
	if ( l.size( ) != r.size( ) )
		return l.size( ) < r.size( );
	return memory::less( l.data( ), r.data( ), l.size( ) );
}

bool operator>( config const& l, config const& r )
{
	if ( l.size( ) != r.size( ) )
		return l.size( ) > r.size( );
	return memory::greater( l.data( ), r.data( ), l.size( ) );
}

bool operator<=( config const& l, config const& r )
{
	if ( l.size( ) != r.size( ) )
		return l.size( ) <= r.size( );
	return !memory::greater( l.data( ), r.data( ), l.size( ) );
}

bool operator>=( config const& l, config const& r )
{
	if ( l.size( ) != r.size( ) )
		return l.size( ) >= r.size( );
	return !memory::less( l.data( ), r.data( ), l.size( ) );
}

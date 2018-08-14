#include "config.h"
#include "memory.h"
#include "hash.h"

config::config( )
{ }

config::config( pointer data, uptr size ) :
	m_data		( data ),
	m_size		( size )
{ }

void config::create( pointer data, uptr size )
{
	m_data		= data;
	m_size		= size;
}

pointer config::data( ) const
{
	return m_data;
}

uptr config::size( ) const
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
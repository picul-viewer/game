#include "reader.h"
#include "macros.h"
#include "strings.h"

lib::reader::reader( pointer const data, uptr const size ) :
	m_pointer		( data ),
	m_end			( data + size )
{ }

void lib::reader::create( pointer const data, uptr const size )
{
	m_pointer		= data;
	m_end			= data + size;
}

void lib::reader::operator+=( uptr const size )
{
	m_pointer		+= size;
	ASSERT_CMP	  ( m_pointer, <=, m_end );
}

pvoid lib::reader::read_data( uptr const size )
{
	pvoid const v	= (pvoid)m_pointer;
	m_pointer		+= size;
	ASSERT_CMP		( m_pointer, <=, m_end );
	return			v;
}

pstr lib::reader::read_str( )
{
	pstr const v	= (pstr)m_pointer;
	m_pointer		+= strings::length( v ) + 1;
	ASSERT_CMP		( m_pointer, <=, m_end );
	return			v;
}

pointer lib::reader::ptr( ) const
{
	return m_pointer;
}

bool lib::reader::is_valid( ) const
{
	return ( m_pointer != nullptr ) && ( m_pointer <= m_end ) ;
}
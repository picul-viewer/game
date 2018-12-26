#include "reader.h"
#include "macros.h"
#include "strings.h"

reader::reader( pointer const data, uptr const size ) :
	m_pointer		( data ),
	m_end			( data + size )
{ }

void reader::create( pointer const data, uptr const size )
{
	m_pointer		= data;
	m_end			= data + size;
}

void reader::operator+=( uptr const size )
{
	m_pointer		+= size;
	ASSERT			( m_pointer <= m_end );
}

pvoid reader::read_data( uptr const size )
{
	pvoid const v	= (pvoid)m_pointer;
	m_pointer		+= size;
	ASSERT			( m_pointer <= m_end );
	return			v;
}

pstr reader::read_str( )
{
	pstr const v	= (pstr)m_pointer;
	m_pointer		+= strings::length( v ) + 1;
	ASSERT			( m_pointer <= m_end );
	return			v;
}

pointer reader::ptr( ) const
{
	return m_pointer;
}

bool reader::is_valid( ) const
{
	return ( m_pointer != nullptr ) && ( m_pointer <= m_end ) ;
}
#include "writer.h"
#include "macros.h"
#include "memory.h"
#include "strings.h"

writer::writer( pointer const data, uptr const size ) :
	m_pointer		( data ),
	m_end			( data + size )
{ }

void writer::create( pointer const data, uptr const size )
{
	m_pointer		= data;
	m_end			= data + size;
}

void writer::operator+=( uptr const size )
{
	m_pointer		+= size;
	ASSERT_CMP		( m_pointer, <=, m_end );
}

void writer::write_data( pcvoid const data, uptr const size )
{
	ASSERT_CMP		( m_pointer + size, <=, m_end );
	memory::copy	( m_pointer, data, size );
	m_pointer		+= size;
}

void writer::write_str( pcstr value )
{
	uptr const l	= strings::length( value ) + 1;
	write_data		( value, l );
}

pointer writer::ptr( ) const
{
	return m_pointer;
}

uptr writer::size( pointer const memory_pointer ) const
{
	return m_pointer - memory_pointer;
}

bool writer::is_valid( ) const
{
	return ( m_pointer != nullptr ) && ( m_pointer <= m_end ) ;
}
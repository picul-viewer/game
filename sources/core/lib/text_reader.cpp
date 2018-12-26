#include "text_reader.h"

text_reader::text_reader( pointer const data, uptr const size ) :
	m_pointer	( data ),
	m_end		( data + size )
{ }

void text_reader::create( pointer const data, uptr const size )
{
	m_pointer	= data;
	m_end		= data + size;
}

void text_reader::read_str( pstr const data )
{
	u32 pos;
	_snscanf( m_pointer, m_end - m_pointer, "%s%n", data, &pos );
	m_pointer += pos;
}

void text_reader::skip_line( )
{
	u32 pos;
	_snscanf( m_pointer, m_end - m_pointer, "%*[^\n]\n%n", &pos );
	m_pointer += pos;
}

pointer text_reader::ptr( ) const
{
	return m_pointer;
}

bool text_reader::is_valid( ) const
{
	return ( m_pointer != nullptr ) && ( m_pointer <= m_end );
}
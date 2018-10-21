#include "text_config.h"

text_config::text_config( pointer data ) :
	m_begin		( data ),
	m_pointer	( data )
{ }

void text_config::set( pointer data )
{
	m_begin		= data;
	m_pointer	= data;
}

void text_config::read_str( pstr data )
{
	u32 pos;
	sscanf( m_pointer, "%s%n", data, &pos );
	m_pointer += pos;
}

bool text_config::eof( ) const
{
	return m_pointer.get<char>( ) == '\0';
}
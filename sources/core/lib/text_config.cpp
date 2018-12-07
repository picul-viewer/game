#include "text_config.h"

text_config::text_config( config const& cfg )
	: text_config( cfg.data( ), cfg.size( ) )
{ }

text_config::text_config( pointer const data, uptr const size ) :
	m_pointer	( data )
	, m_end		( data + size )
{ }

void text_config::create( config const& cfg )
{
	create( cfg.data( ), cfg.size( ) );
}

void text_config::create( pointer const data, uptr const size )
{
	m_pointer	= data;
	m_end		= data + size;
}

void text_config::read_str( pstr const data )
{
	u32 pos;
	_snscanf( m_pointer, m_end - m_pointer, "%s%n", data, &pos );
	m_pointer += pos;
}

void text_config::skip_line( )
{
	u32 pos;
	_snscanf( m_pointer, m_end - m_pointer, "%*[^\n]\n%n", &pos );
	m_pointer += pos;
}

void text_config::write_str( pstr const data )
{
	m_pointer += snprintf( m_pointer, m_end - m_pointer, data );
}

pointer text_config::get_pointer( ) const
{
	return m_pointer;
}

bool text_config::is_valid( ) const
{
	return ( m_pointer != nullptr ) && ( m_pointer <= m_end );
}
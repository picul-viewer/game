#include "text_writer.h"

text_writer::text_writer( pointer const data, uptr const size ) :
	m_pointer	( data ),
	m_end		( data + size )
{ }

void text_writer::create( pointer const data, uptr const size )
{
	m_pointer	= data;
	m_end		= data + size;
}

void text_writer::write_str( pstr const data )
{
	m_pointer += snprintf( m_pointer, m_end - m_pointer, data );
}

pointer text_writer::ptr( ) const
{
	return m_pointer;
}

uptr text_writer::size( pointer const memory_pointer ) const
{
	return m_pointer - memory_pointer;
}

bool text_writer::is_valid( ) const
{
	return ( m_pointer != nullptr ) && ( m_pointer <= m_end );
}
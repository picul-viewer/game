#include "dynamic_writer.h"
#include "macros.h"
#include "memory.h"
#include "strings.h"

void dynamic_writer::create( )
{
	m_allocator.create( );
}

void dynamic_writer::destroy( )
{
	m_allocator.destroy( );
}

void dynamic_writer::operator+=( uptr const size )
{
	m_allocator.allocate( size );
}

void dynamic_writer::write_data( pcvoid const data, uptr const size )
{
	pointer const p	= m_allocator.allocate( size );
	memory::copy	( p, data, size );
}

void dynamic_writer::write_str( pcstr value )
{
	uptr const l	= strings::length( value ) + 1;
	write_data		( value, l );
}

pointer dynamic_writer::data( ) const
{
	return m_allocator.data( );
}

pointer dynamic_writer::ptr( ) const
{
	return m_allocator.data_end( );
}

uptr dynamic_writer::size( ) const
{
	return m_allocator.data_end( ) - m_allocator.data( );
}
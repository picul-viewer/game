#include "binary_config.h"
#include "memory.h"
#include "strings.h"

binary_config::binary_config( config const& cfg ) :
	binary_config( cfg.data( ), cfg.size( ) )
{ }

binary_config::binary_config( pointer const data, uptr const size ) :
	m_pointer		( data )
	, m_end			( data + size )
{ }

void binary_config::create( config const& cfg )
{
	create			( cfg.data( ), cfg.size( ) );
}

void binary_config::create( pointer const data, uptr const size )
{
	m_pointer		= data;
	m_end			= data + size;
}

void binary_config::operator+=( uptr size )
{
	m_pointer		+= size;
	ASSERT			( m_pointer <= m_end );
}

pvoid binary_config::read_data( uptr size )
{
	pvoid const v	= (pvoid)m_pointer;
	m_pointer		+= size;
	ASSERT			( m_pointer <= m_end );
	return			v;
}

pstr binary_config::read_str( )
{
	pstr const v	= (pstr)m_pointer;
	m_pointer		+= strings::length( v ) + 1;
	ASSERT			( m_pointer <= m_end );
	return			v;
}

config binary_config::read_config( )
{
	u32 const size	= read<u32>( );
	config const cfg( m_pointer, size );
	m_pointer		+= size;
	ASSERT			( m_pointer <= m_end );
	return			cfg;
}

void binary_config::write_data( pcvoid data, uptr size )
{
	ASSERT			( m_pointer + size <= m_end );
	memory::copy	( m_pointer, data, size );
	m_pointer		+= size;
}

void binary_config::write_str( pcstr value )
{
	uptr const l	= strings::length( value ) + 1;
	ASSERT			( m_pointer + l <= m_end );
	strings::copy_n	( m_pointer, value, l + 1 );
	m_pointer		+= l;
}

void binary_config::write_config( config const& cfg )
{
	ASSERT			( cfg.size( ) < 0x100000000ULL );
	u32 const size	= (u32)cfg.size( );
	write<u32>		( size );
	ASSERT			( m_pointer + size <= m_end );
	strings::copy_n	( m_pointer, cfg.data( ), size );
	m_pointer		+= size;
}

pointer binary_config::get_pointer( ) const
{
	return m_pointer;
}

bool binary_config::is_valid( ) const
{
	return ( m_pointer != nullptr ) && ( m_pointer <= m_end ) ;
}
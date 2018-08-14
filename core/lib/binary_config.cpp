#include "binary_config.h"
#include "memory.h"
#include "strings.h"

binary_config::binary_config( )
{ }

binary_config::binary_config( config const& cfg ) :
	binary_config( cfg.data( ), cfg.size( ) )
{ }

binary_config::binary_config( pointer data, uptr size ) :
	m_data			( data )
	
#ifdef DEBUG
	, m_end			( data + size )
#endif // #ifdef DEBUG
{ }

void binary_config::create( config const& cfg )
{
	create			( cfg.data( ), cfg.size( ) );
}

void binary_config::create( pointer data, uptr size )
{
	m_data			= data;

#ifdef DEBUG
	m_end			= data + size;
#endif // #ifdef DEBUG
}

void binary_config::operator+=( uptr size )
{
	m_data			+= size;
	
#ifdef DEBUG
	ASSERT			( m_data <= m_end );
#endif // #ifdef DEBUG
}

pstr binary_config::read_str( )
{
	pstr value		= (pstr)m_data;
	m_data			+= strings::length( value ) + 1;
	
#ifdef DEBUG
	ASSERT			( m_data <= m_end );
#endif // #ifdef DEBUG

	return			value;
}

config binary_config::read_config( )
{
	u32 size		= read<u32>( );

	config cfg		( m_data, size );
	m_data			+= size;
	
#ifdef DEBUG
	ASSERT			( m_data <= m_end );
#endif // #ifdef DEBUG

	return			cfg;
}

void binary_config::write_str( pcstr value )
{
	uptr l			= strings::length( value ) + 1;

#ifdef DEBUG
	ASSERT			( m_data + l <= m_end );
#endif // #ifdef DEBUG

	memory::copy	( m_data, value, ( l + 1 ) * sizeof(char) );
	m_data			+= l;
}

void binary_config::write_config( config const& cfg )
{
	ASSERT			( cfg.size( ) < 0x100000000ULL );
	u32 size		= (u32)cfg.size( );
	write<u32>		( size );

#ifdef DEBUG
	ASSERT			( m_data + size <= m_end );
#endif // #ifdef DEBUG

	memory::copy	( m_data, cfg.data( ), size );
	m_data			+= size;
}

pointer binary_config::get_pointer( ) const
{
	return m_data;
}
#ifndef __core_config_h_included_
#define __core_config_h_included_

#include <types.h>
#include <lib/strings.h>
#include <lib/memory.h>

class config
{
public:
	config( pointer data, uptr size );
	void create( pointer data, uptr size );

	void operator+=( ptr size );
	void reset( );

	template<typename T>
	inline T const& read( );
	
	inline pstr read_str( );
	
	template<typename T>
	inline void write( T const& value );
	
	inline void write_str( pcstr value );
	
	pointer data( ) const;
	pointer current( ) const;
	uptr size( ) const;
	uptr capacity( ) const;

	u32 hash( ) const;

protected:
	pointer	m_data;
	pointer	m_pointer;
	uptr m_size;
};

bool operator==( config const& l, config const& r );
bool operator!=( config const& l, config const& r );
bool operator<( config const& l, config const& r );
bool operator>( config const& l, config const& r );
bool operator<=( config const& l, config const& r );
bool operator>=( config const& l, config const& r );

template<typename T>
T const& config::read( )
{
	T& value	= *(T*)m_pointer;
	m_pointer	+= sizeof(T);
	ASSERT		( m_pointer < m_data + m_size );
	return		value;
}

pstr config::read_str( )
{
	pstr value	= (pstr)m_pointer;
	m_pointer	+= strings::length( value ) + 1;
	ASSERT		( m_pointer < m_data + m_size );
	return		value;
}

template<typename T>
void config::write( T const& value )
{
	ASSERT			( m_pointer + sizeof(T) < m_data + m_size );
	*(T*)m_pointer	= value;
	m_pointer		+= sizeof(T);
}

void config::write_str( pcstr value )
{
	uptr l			= strings::length( value ) + 1;
	ASSERT			( m_pointer + l < m_data + m_size );
	memory::copy	( (char*)m_pointer, value, ( l + 1 ) );
	m_pointer		+= l;
}

#endif // #ifndef __core_config_h_included_
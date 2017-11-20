#ifndef __core_config_h_included_
#define __core_config_h_included_

#include <core/types.h>
#include <core/structs.h>

class config
{
public:
	config( pointer data, uptr size );
	void set( pointer data, uptr size );

	void move( ptr size );

	template<typename T>
	inline T const& read( );
	
	inline pstr read_str( );

protected:
	pointer	m_begin;
#ifdef DEBUG
	pointer	m_end;
#endif // #ifdef DEBUG
	pointer	m_pointer;
};

template<typename T>
T const& config::read( )
{
	T& value	= *(T*)m_pointer;
	m_pointer	+= sizeof(T);
	ASSERT		( m_pointer < m_end );
	return		value;
}

pstr config::read_str( )
{
	pstr value	= (pstr)m_pointer;
	m_pointer	+= weak_const_string( value ).length( ) + 1;
	ASSERT		( m_pointer < m_end );
	return		value;
}

class config_builder
{
public:
	config_builder( pointer data, uptr max_size );
	void set( pointer data, uptr max_size );

	template<typename T>
	inline void write( T const& value );
	
	inline void write_str( pcstr value );

	pointer data( ) const;
	uptr size( ) const;

protected:
	pointer	m_begin;
#ifdef DEBUG
	pointer	m_end;
#endif // #ifdef DEBUG
	pointer	m_pointer;
};

template<typename T>
void config_builder::write( T const& value )
{
	ASSERT			( m_pointer + sizeof(T) < m_end );
	*(T*)m_pointer	= value;
	m_pointer		+= sizeof(T);
}

void config_builder::write_str( pcstr value )
{
	uptr l			= weak_const_string( value ).length( ) + 1;
	ASSERT			( m_pointer + l < m_end );
	strncpy			( (char*)m_pointer, value, l );
	m_pointer		+= l;
}

#endif // #ifndef __core_config_h_included_
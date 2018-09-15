#ifndef __core_binary_config_h_included_
#define __core_binary_config_h_included_

#include <types.h>
#include "macros.h"
#include "config.h"

class binary_config
{
public:
	binary_config( );

	binary_config( config const& cfg );
	binary_config( pointer data, uptr size );
	void create( config const& cfg );
	void create( pointer data, uptr size );

	void operator+=( uptr size );

	template<typename T>
	inline T const& read( );
	
	pvoid read_data( uptr size );
	pstr read_str( );
	config read_config( );
	
	template<typename T>
	inline void write( T const& value );
	
	void write_data( pcvoid data, uptr size );
	void write_str( pcstr value );
	void write_config( config const& cfg );
	
	pointer get_pointer( ) const;

	bool is_valid( ) const;

protected:
	pointer	m_data;

#ifdef DEBUG
	pointer m_end;
#endif // #ifdef DEBUG

};

template<typename T>
T const& binary_config::read( )
{
	T& value	= *(T*)m_data;
	m_data		+= sizeof(T);

#ifdef DEBUG
	ASSERT		( m_data <= m_end );
#endif // #ifdef DEBUG

	return		value;
}

template<typename T>
void binary_config::write( T const& value )
{
#ifdef DEBUG
	ASSERT		( m_data + sizeof(T) <= m_end );
#endif // #ifdef DEBUG

	*(T*)m_data	= value;
	m_data		+= sizeof(T);
}

#endif // #ifndef __core_config_h_included_
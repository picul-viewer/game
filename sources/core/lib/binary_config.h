#ifndef __core_binary_config_h_included_
#define __core_binary_config_h_included_

#include <types.h>
#include "macros.h"
#include "config.h"

class binary_config
{
public:
	binary_config( ) = default;

	binary_config( config const& cfg );
	binary_config( pointer const data, uptr const size );
	void create( config const& cfg );
	void create( pointer const data, uptr const size );

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
	pointer	m_pointer;
	pointer m_end;

};

template<typename T>
T const& binary_config::read( )
{
	T& value		= *(T*)m_pointer;
	m_pointer		+= sizeof(T);
	ASSERT			( m_pointer <= m_end );
	return			value;
}

template<typename T>
void binary_config::write( T const& value )
{
	ASSERT			( m_pointer + sizeof(T) <= m_end );
	*(T*)m_pointer	= value;
	m_pointer		+= sizeof(T);
}

#endif // #ifndef __core_config_h_included_
#ifndef __core_config_h_included_
#define __core_config_h_included_

#include <types.h>

class config
{
public:
	config( pointer data, uptr size );
	void create( pointer data, uptr size );

	pointer data( ) const;
	uptr size( ) const;

	u32 hash( ) const;

protected:
	pointer	m_data;
	uptr m_size;
};

bool operator==( config const& l, config const& r );
bool operator!=( config const& l, config const& r );
bool operator<( config const& l, config const& r );
bool operator>( config const& l, config const& r );
bool operator<=( config const& l, config const& r );
bool operator>=( config const& l, config const& r );

#endif // #ifndef __core_config_h_included_
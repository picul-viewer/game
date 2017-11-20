#ifndef __core_basic_string_h_included_
#define __core_basic_string_h_included_

#include <core/types.h>

class i_const_string
{
public:
	inline uptr length( ) const;

	inline bool empty( ) const;
	
	inline char operator[]( uptr index ) const;
	inline char at( uptr index ) const;
	
	virtual inline const char* c_str( ) const = 0;
	
	inline uptr find( const char* str, uptr pos = 0 ) const;
	inline uptr find( char c, uptr pos = 0 ) const;
};

bool operator==( i_const_string const& left, i_const_string const& right );
bool operator!=( i_const_string const& left, i_const_string const& right );
bool operator<=( i_const_string const& left, i_const_string const& right );
bool operator>=( i_const_string const& left, i_const_string const& right );
bool operator<( i_const_string const& left, i_const_string const& right );
bool operator>( i_const_string const& left, i_const_string const& right );

class i_string : public i_const_string
{
public:
	void clear( );
	
	char& operator[]( uptr index );
	char& at( uptr index );
	
	virtual char* data( ) const = 0;
};

#endif // #ifndef __core_basic_string_h_included_

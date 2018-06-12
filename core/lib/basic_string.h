#ifndef __core_basic_string_h_included_
#define __core_basic_string_h_included_

#include <types.h>

template<typename StringClass>
class i_const_string
{
public:
	uptr length( ) const;

	u32 hash( ) const;

	bool empty( ) const;
	
	char operator[]( uptr index ) const;
	char at( uptr index ) const;
	
	char const* c_str( ) const;

	uptr find( const char* str, uptr pos = 0 ) const;
	uptr find( char c, uptr pos = 0 ) const;
};

template<typename LStringClass, typename RStringClass>
bool operator==( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right );
template<typename LStringClass, typename RStringClass>
bool operator!=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right );
template<typename LStringClass, typename RStringClass>
bool operator<=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right );
template<typename LStringClass, typename RStringClass>
bool operator>=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right );
template<typename LStringClass, typename RStringClass>
bool operator<( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right );
template<typename LStringClass, typename RStringClass>
bool operator>( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right );

template<typename StringClass>
class i_string : public i_const_string<StringClass>
{
public:
	void clear( );

	void resize( uptr size );
	
	char& operator[]( uptr index );
	char& at( uptr index );
	
	char* data( ) const;

	void replace( char value, char new_value );
};

#include "basic_string_inline.h"

#endif // #ifndef __core_basic_string_h_included_

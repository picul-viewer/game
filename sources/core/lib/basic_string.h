#ifndef GUARD_CORE_BASIC_STRING_H_INCLUDED
#define GUARD_CORE_BASIC_STRING_H_INCLUDED

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
	
	//pcstr c_str( ) const; - needs to be declared in implementation
	operator pcstr( ) const;

	uptr find( pcstr str, uptr pos = 0 ) const;
	uptr find( char c, uptr pos = 0 ) const;
	
	uptr rfind( char c ) const;
	uptr rfind( char c, uptr pos ) const;

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

template<typename LStringClass>
bool operator==( i_const_string<LStringClass> const& left, pcstr right );
template<typename LStringClass>
bool operator!=( i_const_string<LStringClass> const& left, pcstr right );
template<typename LStringClass>
bool operator<=( i_const_string<LStringClass> const& left, pcstr right );
template<typename LStringClass>
bool operator>=( i_const_string<LStringClass> const& left, pcstr right );
template<typename LStringClass>
bool operator<( i_const_string<LStringClass> const& left, pcstr right );
template<typename LStringClass>
bool operator>( i_const_string<LStringClass> const& left, pcstr right );

template<typename RStringClass>
bool operator==( pcstr left, i_const_string<RStringClass> const& right );
template<typename RStringClass>
bool operator!=( pcstr left, i_const_string<RStringClass> const& right );
template<typename RStringClass>
bool operator<=( pcstr left, i_const_string<RStringClass> const& right );
template<typename RStringClass>
bool operator>=( pcstr left, i_const_string<RStringClass> const& right );
template<typename RStringClass>
bool operator<( pcstr left, i_const_string<RStringClass> const& right );
template<typename RStringClass>
bool operator>( pcstr left, i_const_string<RStringClass> const& right );

template<typename StringClass>
class i_string : public i_const_string<StringClass>
{
public:
	void clear( );

	void resize( uptr size );
	
	char& operator[]( uptr index );
	char& at( uptr index );
	
	//pstr data( ); - needs to be declared in implementation
	operator pstr( );

	void replace( char value, char new_value );
};

#include "basic_string_inline.h"

#endif // #ifndef GUARD_CORE_BASIC_STRING_H_INCLUDED

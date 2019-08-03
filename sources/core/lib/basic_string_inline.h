#ifndef GUARD_CORE_BASIC_STRING_INLINE_H_INCLUDED
#define GUARD_CORE_BASIC_STRING_INLINE_H_INCLUDED

#include <macros.h>
#include "strings.h"
#include "hash.h"

template<typename StringClass>
uptr i_const_string<StringClass>::length( ) const
{
	return strings::length( static_cast<StringClass const*>( this )->c_str( ) );
}

template<typename StringClass>
u32 i_const_string<StringClass>::hash( ) const
{
	StringClass const* this_casted = static_cast<StringClass const*>( this );
	ASSERT( this_casted->c_str( ) );

	return lib::hash32( this_casted->c_str( ), this_casted->length( ) );
}

template<typename StringClass>
bool i_const_string<StringClass>::empty( ) const
{
	return strings::empty( static_cast<StringClass const*>( this )->c_str( ) );
}

template<typename StringClass>
char i_const_string<StringClass>::operator[]( uptr index ) const
{
	return at( index );
}

template<typename StringClass>
char i_const_string<StringClass>::at( uptr index ) const
{
	StringClass const* this_casted = static_cast<StringClass const*>( this );
	ASSERT( this_casted->c_str( ) );

	ASSERT_CMP( index, <, this_casted->length( ) );
	return this_casted->c_str( )[index];
}

template<typename StringClass>
inline i_const_string<StringClass>::operator pcstr( ) const
{
	return static_cast<StringClass const*>( this )->c_str( );
}

template<typename StringClass>
inline uptr i_const_string<StringClass>::find( pcstr str, uptr pos ) const
{
	StringClass const* this_casted = static_cast<StringClass const*>( this );

	return strings::find( this_casted->c_str( ), str, pos );
}

template<typename StringClass>
inline uptr i_const_string<StringClass>::find( char c, uptr pos ) const
{
	StringClass const* this_casted = static_cast<StringClass const*>( this );
	
	return strings::find( this_casted->c_str( ), c, pos );
}

template<typename StringClass>
inline uptr i_const_string<StringClass>::rfind( char c ) const
{
	StringClass const* this_casted = static_cast<StringClass const*>( this );

	return strings::rfind( this_casted->c_str( ), c );
}

template<typename StringClass>
inline uptr i_const_string<StringClass>::rfind( char c, uptr pos ) const
{
	StringClass const* this_casted = static_cast<StringClass const*>( this );
	
	return strings::rfind( this_casted->c_str( ), c, pos );
}


template<typename LStringClass, typename RStringClass>
bool operator==( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), static_cast<RStringClass const&>( right ).c_str( ) ) == 0;
}

template<typename LStringClass, typename RStringClass>
bool operator!=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), static_cast<RStringClass const&>( right ).c_str( ) ) != 0;
}

template<typename LStringClass, typename RStringClass>
bool operator<=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), static_cast<RStringClass const&>( right ).c_str( ) ) <= 0;
}

template<typename LStringClass, typename RStringClass>
bool operator>=( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), static_cast<RStringClass const&>( right ).c_str( ) ) >= 0;
}

template<typename LStringClass, typename RStringClass>
bool operator<( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), static_cast<RStringClass const&>( right ).c_str( ) ) < 0;
}

template<typename LStringClass, typename RStringClass>
bool operator>( i_const_string<LStringClass> const& left, i_const_string<RStringClass> const& right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), static_cast<RStringClass const&>( right ).c_str( ) ) > 0;
}

template<typename LStringClass>
bool operator==( i_const_string<LStringClass> const& left, pcstr right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), right ) == 0;
}

template<typename LStringClass>
bool operator!=( i_const_string<LStringClass> const& left, pcstr right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), right ) != 0;
}

template<typename LStringClass>
bool operator<=( i_const_string<LStringClass> const& left, pcstr right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), right ) <= 0;
}

template<typename LStringClass>
bool operator>=( i_const_string<LStringClass> const& left, pcstr right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), right ) >= 0;
}

template<typename LStringClass>
bool operator<( i_const_string<LStringClass> const& left, pcstr right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), right ) < 0;
}

template<typename LStringClass>
bool operator>( i_const_string<LStringClass> const& left, pcstr right )
{
	return strings::compare( static_cast<LStringClass const&>( left ).c_str( ), right ) > 0;
}

template<typename RStringClass>
bool operator==( pcstr left, i_const_string<RStringClass> const& right )
{
	return strings::compare( left, static_cast<RStringClass const&>( right ).c_str( ) ) == 0;
}

template<typename RStringClass>
bool operator!=( pcstr left, i_const_string<RStringClass> const& right )
{
	return strings::compare( left, static_cast<RStringClass const&>( right ).c_str( ) ) != 0;
}

template<typename RStringClass>
bool operator<=( pcstr left, i_const_string<RStringClass> const& right )
{
	return strings::compare( left, static_cast<RStringClass const&>( right ).c_str( ) ) <= 0;
}

template<typename RStringClass>
bool operator>=( pcstr left, i_const_string<RStringClass> const& right )
{
	return strings::compare( left, static_cast<RStringClass const&>( right ).c_str( ) ) >= 0;
}

template<typename RStringClass>
bool operator<( pcstr left, i_const_string<RStringClass> const& right )
{
	return strings::compare( left, static_cast<RStringClass const&>( right ).c_str( ) ) < 0;
}

template<typename RStringClass>
bool operator>( pcstr left, i_const_string<RStringClass> const& right )
{
	return strings::compare( left, static_cast<RStringClass const&>( right ).c_str( ) ) > 0;
}


template<typename StringClass>
void i_string<StringClass>::clear( )
{
	StringClass* this_casted = static_cast<StringClass*>( this );
	ASSERT( this_casted->data( ) );

	this_casted->data( )[0] = '\0';
}

template<typename StringClass>
void i_string<StringClass>::resize( uptr size )
{
	StringClass* this_casted = static_cast<StringClass*>( this );
	ASSERT( this_casted->data( ) );

	this_casted->data( )[size] = '\0';
}

template<typename StringClass>
char& i_string<StringClass>::operator[]( uptr index )
{
	return at( index );
}

template<typename StringClass>
char& i_string<StringClass>::at( uptr index )
{
	StringClass* this_casted = static_cast<StringClass*>( this );
	ASSERT( this_casted->data( ) );

	ASSERT_CMP( index, <, this_casted->length( ) );
	return this_casted->data( )[index];
}

template<typename StringClass>
inline i_string<StringClass>::operator pstr( )
{
	return static_cast<StringClass*>( this )->data( );
}

template<typename StringClass>
inline void i_string<StringClass>::replace( char value, char new_value )
{
	StringClass* this_casted = static_cast<StringClass*>( this );
	ASSERT( this_casted->data( ) );

	for ( uptr i = 0, l = this_casted->length( ); i != l; ++i )
		if ( this_casted->data( )[i] == value )
			this_casted->data( )[i] = new_value;
}

#endif // #ifndef GUARD_CORE_BASIC_STRING_INLINE_H_INCLUDED
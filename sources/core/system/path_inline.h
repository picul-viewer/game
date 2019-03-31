#ifndef GUARD_CORE_PATH_INLINE_H_INCLUDED
#define GUARD_CORE_PATH_INLINE_H_INCLUDED

namespace sys {

template<typename StringClass>
path::path( i_const_string<StringClass> const& str ) :
	path( static_cast<StringClass const&>( str ).c_str( ) )
{ }

template<typename StringClass>
path path::operator+( i_const_string<StringClass> const& str ) const
{
	StringClass const& str_casted = static_cast<StringClass const&>( str );
	ASSERT( str_casted.c_str( ) );

	return *this + str_casted.c_str( );
}

template<typename StringClass>
path& path::operator+=( i_const_string<StringClass> const& str )
{
	StringClass const& str_casted = static_cast<StringClass const&>( str );
	ASSERT( str_casted.c_str( ) );

	return *this += str_casted.c_str( );
}

} // namespace sys

#endif // #ifndef GUARD_CORE_PATH_INLINE_H_INCLUDED
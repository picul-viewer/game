#ifndef GUARD_LIB_WEAK_STRING_INLINE_H_INCLUDED
#define GUARD_LIB_WEAK_STRING_INLINE_H_INCLUDED

template<typename StringClass>
weak_const_string::weak_const_string( i_const_string<StringClass> const& str ) :
	m_data( static_cast<StringClass const&>( str ).c_str( ) )
{ }


template<typename StringClass>
weak_string::weak_string( i_string<StringClass> const& str ) :
	m_data( static_cast<StringClass&>( str ).data( ) )
{ }

#endif // #ifndef GUARD_LIB_WEAK_STRING_INLINE_H_INCLUDED
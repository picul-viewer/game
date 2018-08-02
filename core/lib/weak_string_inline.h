#ifndef __lib_weak_string_inline_h_included_
#define __lib_weak_string_inline_h_included_

template<typename StringClass>
weak_const_string::weak_const_string( i_const_string<StringClass> const& str ) :
	m_data( static_cast<StringClass const&>( str ).c_str( ) )
{ }


template<typename StringClass>
weak_string::weak_string( i_string<StringClass> const& str ) :
	m_data( static_cast<StringClass&>( str ).data( ) )
{ }

#endif // #ifndef __lib_weak_string_inline_h_included_
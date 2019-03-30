#ifndef __core_fixed_array_inline_h_included_
#define __core_fixed_array_inline_h_included_

#include <macros.h>

template<typename T, uptr Size>
T* lib::fixed_array<T, Size>::data( )
{
	return begin( );
}

template<typename T, uptr Size>
T const* lib::fixed_array<T, Size>::data( ) const
{
	return begin( );
}

template<typename T, uptr Size>
uptr lib::fixed_array<T, Size>::size( ) const
{
	return Size;
}

template<typename T, uptr Size>
T* lib::fixed_array<T, Size>::begin( )
{
	return &(T&)m_data[0];
}

template<typename T, uptr Size>
T const* lib::fixed_array<T, Size>::begin( ) const
{
	return &(T&)m_data[0];
}

template<typename T, uptr Size>
T* lib::fixed_array<T, Size>::end( )
{
	return begin( ) + Size;
}

template<typename T, uptr Size>
T const* lib::fixed_array<T, Size>::end( ) const
{
	return begin( ) + Size;
}
	
template<typename T, uptr Size>
T& lib::fixed_array<T, Size>::at( uptr const index )
{
	ASSERT_CMP( index, <, Size );
	return m_data[index];
}

template<typename T, uptr Size>
T const& lib::fixed_array<T, Size>::at( uptr const index ) const
{
	ASSERT_CMP( index, <, Size );
	return m_data[index];
}

template<typename T, uptr Size>
T& lib::fixed_array<T, Size>::operator[]( uptr const index )
{
	return at( index );
}

template<typename T, uptr Size>
T const& lib::fixed_array<T, Size>::operator[]( uptr const index ) const
{
	return at( index );
}

template<typename T, uptr Size>
template<typename Pred>
void lib::fixed_array<T, Size>::for_each( Pred const& functor ) const
{
	for ( T* i = m_data; i != m_data + Size; ++i )
		functor( i );
}

#endif // #ifndef __core_fixed_array_inline_h_included_
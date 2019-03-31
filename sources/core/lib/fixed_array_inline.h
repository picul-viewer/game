#ifndef GUARD_CORE_FIXED_ARRAY_INLINE_H_INCLUDED
#define GUARD_CORE_FIXED_ARRAY_INLINE_H_INCLUDED

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

#endif // #ifndef GUARD_CORE_FIXED_ARRAY_INLINE_H_INCLUDED
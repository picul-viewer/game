#ifndef GUARD_CORE_LINEAR_INLINE_SET_H_INCLUDED
#define GUARD_CORE_LINEAR_INLINE_SET_H_INCLUDED

#include <macros.h>
#include <lib/algorithms.h>
#include <lib/allocator.h>
#include <lib/hash.h>

namespace lib {

template<typename T, u32 TableSize>
void linear_set<T, TableSize>::create( pointer const in_allocated_memory )
{
	pointer const memory =
		( in_allocated_memory == nullptr ) ?
		virtual_allocator( ).allocate( memory_size ) :
		in_allocated_memory;

	m_key_table = memory;
	m_value_table = memory + key_table_memory_size;
}

template<typename T, u32 TableSize>
void linear_set<T, TableSize>::destroy( )
{
	virtual_allocator( ).deallocate( m_key_table );
}

template<typename T, u32 TableSize>
T* linear_set<T, TableSize>::find( T const value ) const
{
	return find_if( hash( value ), [&value]( T const& v )
	{
		return v == value;
	} );
}

template<typename T, u32 TableSize>
bool linear_set<T, TableSize>::find_or_insert( T const value, T*& found_value )
{
	return find_if_or_insert(
		hash( value ),
		[&value]( T const& v )
		{
			return v == value;
		},
		[&value]( )
		{
			return lib::move( value );
		},
		found_value
	);
}

template<typename T, u32 TableSize>
template<typename Functor>
T* linear_set<T, TableSize>::find_if( u32 const hash, Functor const& f ) const
{
	u32 const max_distance = ( TableSize < 256 ) ? TableSize : 255;

	u32 const start_index = hash % TableSize;
	u32 index = start_index;

	for ( u8 i = 1; ; ++i )
	{
		u8 const current_key = m_key_table[index];

		if ( i == current_key )
		{
			T const current_value = m_value_table[index];

			if ( f( current_value ) )
			{
				return m_value_table + index;
			}
		}

		if ( i > current_key )
			break;

		index = ( index + 1 ) % TableSize;

		ASSERT( i != max_distance, "set is full" );
	}

	return nullptr;
}

template<typename T, u32 TableSize>
template<typename Functor, typename InsertValueGenerator>
bool linear_set<T, TableSize>::find_if_or_insert( u32 const hash, Functor const& f, InsertValueGenerator const& value_gen, T*& found_value )
{
	u32 const max_distance = ( TableSize < 256 ) ? TableSize : 255;

	u32 const start_index = hash % TableSize;
	u32 index = start_index;

	u8 current_key;
	u8 i;

	for ( i = 1; ; ++i )
	{
		current_key = m_key_table[index];

		if ( i == current_key )
		{
			T const current_value = m_value_table[index];

			if ( f( current_value ) )
			{
				found_value = m_value_table + index;
				return true;
			}
		}

		if ( i > current_key )
			break;

		index = ( index + 1 ) % TableSize;

		ASSERT( i != max_distance, "set is full" );
	}
	
	u8 key_to_insert = current_key;
	T value_to_insert = m_value_table[index];

	m_key_table[index] = i;
	m_value_table[index] = value_gen( );

	found_value = m_value_table + index;

	while ( key_to_insert != 0 )
	{
		index = ( index + 1 ) % TableSize;

		current_key = m_key_table[index];

		ASSERT( ( key_to_insert & 0xFF ) != 0xFF, "unable to insert ( too long offset )" );
		++key_to_insert;

		if ( current_key < key_to_insert )
		{
			lib::swap( m_key_table[index], key_to_insert );
			lib::swap( m_value_table[index], value_to_insert );
		}
	}

	return false;
}

template<typename T, u32 TableSize>
T* linear_set<T, TableSize>::insert( T const value )
{
	return insert( hash( value ), value );
}

template<typename T, u32 TableSize>
T* linear_set<T, TableSize>::insert( u32 const hash, T const value )
{
	u32 const max_distance = ( TableSize < 256 ) ? TableSize : 255;

	u32 const start_index = hash % TableSize;
	u32 index = start_index;

	u8 current_key;
	u8 i;

	for ( i = 1; ; ++i )
	{
		current_key = m_key_table[index];

		if ( i > current_key )
			break;

		index = ( index + 1 ) % TableSize;

		ASSERT( i != max_distance, "set is full" );
	}
	
	u8 key_to_insert = current_key;
	T value_to_insert = m_value_table[index];

	m_key_table[index] = i;
	m_value_table[index] = value;

	T* const result = m_value_table + index;

	while ( key_to_insert != 0 )
	{
		index = ( index + 1 ) % TableSize;

		current_key = m_key_table[index];

		ASSERT( ( key_to_insert & 0xFF ) != 0xFF, "unable to insert ( too long offset )" );
		++key_to_insert;

		if ( current_key < key_to_insert )
		{
			lib::swap( m_key_table[index], key_to_insert );
			lib::swap( m_value_table[index], value_to_insert );
		}
	}

	return result;
}

template<typename T, u32 TableSize>
void linear_set<T, TableSize>::remove( T const value )
{
	remove_if( hash( value ), [&value]( T const& v )
	{
		return v == value;
	} );
}

template<typename T, u32 TableSize>
template<typename Functor>
void linear_set<T, TableSize>::remove_if( u32 const hash, Functor const& f )
{
	u32 const start_index = hash % TableSize;
	u32 index = start_index;

	u8 current_key;
	T current_value;
	u8 i;

	for ( i = 1; ; ++i )
	{
		current_key = m_key_table[index];

		if ( i == current_key )
		{
			T const current_value = m_value_table[index];

			if ( f( current_value ) )
				break;
		}

		ASSERT( i <= current_key, "element does not exist" );

		index = ( index + 1 ) % TableSize;
	}

	while ( true )
	{
		u32 const next_index = ( index + 1 ) % TableSize;

		current_key = m_key_table[next_index];
		current_value = m_value_table[next_index];

		if ( current_key <= 1 )
			break;

		m_key_table[index] = current_key - 1;
		m_value_table[index] = current_value;

		index = next_index;
	}

	m_key_table[index] = 0;
	m_value_table[index] = 0;
}

} // namespace lib

#endif // #ifndef GUARD_CORE_LINEAR_INLINE_SET_H_INCLUDED
#ifndef GUARD_CORE_LINEAR_SET_H_INCLUDED
#define GUARD_CORE_LINEAR_SET_H_INCLUDED

#include <types.h>

namespace lib {

template<typename T, u32 TableSize>
class linear_set
{
public:
	void create( pointer const in_allocated_memory = nullptr );
	void destroy( );

	T* find( T const value ) const;
	bool find_or_insert( T const value, T*& found_value );

	template<typename Functor>
	T* find_if( u32 const hash, Functor const& f ) const;
	template<typename Functor, typename InsertValueGenerator>
	bool find_if_or_insert( u32 const hash, Functor const& f, InsertValueGenerator const& value_gen, T*& found_value );

	T* insert( T const value );
	T* insert( u32 const hash, T const value );

	void remove( T const value );
	template<typename Functor>
	void remove_if( u32 const hash, Functor const& f );

public:
	enum : uptr {
		key_table_memory_size = TableSize * sizeof(u8),
		value_table_memory_size = TableSize * sizeof(T),

		memory_size = key_table_memory_size + value_table_memory_size
	};

private:
	u8* m_key_table;
	T* m_value_table;

};

} // namespace lib

#include "linear_set_inline.h"

#endif // #ifndef GUARD_CORE_LINEAR_SET_H_INCLUDED
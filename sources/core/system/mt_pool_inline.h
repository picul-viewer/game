#ifndef GUARD_CORE_MT_POOL_INLINE_H_INCLUDED
#define GUARD_CORE_MT_POOL_INLINE_H_INCLUDED

#include <lib/allocator.h>

template<typename T, u32 MemorySize>
void sys::mt_pool<T, MemorySize>::create( pointer const allocated_memory )
{
	static_assert( MemorySize % Memory_Page_Size == 0, "wrong memory size" );
	static_assert( sizeof(u32) <= sizeof(T), "too small element size" );
	static_assert( sizeof(T) < MemorySize, "too large element size" );

	m_data = ( allocated_memory == nullptr ) ? virtual_allocator( ).allocate( MemorySize ) : allocated_memory;

	m_push_pointer = null_value;
	m_last_pointer = 0;
}

template<typename T, u32 MemorySize>
void sys::mt_pool<T, MemorySize>::destroy( )
{
	virtual_allocator( ).deallocate( m_data );
}

template<typename T, u32 MemorySize>
T* sys::mt_pool<T, MemorySize>::allocate( )
{
	u32 push_pointer_value;
	u32 push_pointer_later_value = m_push_pointer;

	if ( push_pointer_later_value != null_value )
	{
		do
		{
			push_pointer_value = push_pointer_later_value;
			u32 const next_empty_slot = *(u32*)&operator[]( push_pointer_value );
			push_pointer_later_value = interlocked_compare_exchange( m_push_pointer, next_empty_slot, push_pointer_value );
		}
		while ( ( push_pointer_later_value != null_value ) && ( push_pointer_later_value != push_pointer_value ) );

		if ( push_pointer_later_value != null_value )
		{
			T* const result = &operator[]( push_pointer_later_value );
			ASSERT( contains_pointer( result ) );

			return result;
		}
	}

	u32 const new_last_pointer = interlocked_inc( m_last_pointer );

	T* const result = &operator[]( new_last_pointer - 1 );
	ASSERT( contains_pointer( result ) );

	return result;
}

template<typename T, u32 MemorySize>
void sys::mt_pool<T, MemorySize>::deallocate( T* const p )
{
	u32 const index = index_of( p );
	u32* const next_pointer = (u32*)p;

	u32 push_pointer_value;
	u32 push_pointer_later_value = m_push_pointer;

	do
	{
		push_pointer_value = push_pointer_later_value;
		*next_pointer = push_pointer_value;
		push_pointer_later_value = interlocked_compare_exchange( m_push_pointer, index, push_pointer_value );
	}
	while ( push_pointer_value != push_pointer_later_value );
}

template<typename T, u32 MemorySize>
T& sys::mt_pool<T, MemorySize>::operator[]( u32 const index ) const
{
	T* const result = m_data + index;
	ASSERT( contains_pointer( result ) );

	return *result;
}

template<typename T, u32 MemorySize>
u32 sys::mt_pool<T, MemorySize>::index_of( T* const p ) const
{
	ASSERT( contains_pointer( p ) );
	ASSERT( ( (uptr)p - (uptr)m_data ) % sizeof(T) == 0 );

	return (u32)( p - m_data );
}

template<typename T, u32 MemorySize>
bool sys::mt_pool<T, MemorySize>::contains_pointer( pointer const p ) const
{
	return ( p >= (pointer)m_data ) && ( p + sizeof(T) <= (pointer)m_data + MemorySize );
}


template<typename T, u32 MemorySize, u32 PageSize>
void sys::mt_dynamic_pool<T, MemorySize, PageSize>::create( pointer const reserved_memory )
{
	static_assert( MemorySize % Memory_Page_Size == 0, "wrong memory size" );
	static_assert( PageSize % Memory_Page_Size == 0, "wrong page size" );
	static_assert( MemorySize % PageSize == 0, "wrong memory/page size" );

	static_assert( sizeof(u32) <= sizeof(T), "too small element size" );
	static_assert( sizeof(T) < PageSize, "too large element size" );

	m_data = ( reserved_memory == nullptr ) ? virtual_allocator( ).reserve( nullptr, MemorySize ) : reserved_memory;

	m_push_pointer = null_value;
	m_last_pointer = 0;
	m_page_pointer = 0;
}

template<typename T, u32 MemorySize, u32 PageSize>
void sys::mt_dynamic_pool<T, MemorySize, PageSize>::destroy( )
{
	virtual_allocator( ).release( m_data );
}

template<typename T, u32 MemorySize, u32 PageSize>
T* sys::mt_dynamic_pool<T, MemorySize, PageSize>::allocate( )
{
	u32 push_pointer_value;
	u32 push_pointer_later_value = m_push_pointer;

	if ( push_pointer_later_value != null_value )
	{
		do
		{
			push_pointer_value = push_pointer_later_value;
			u32 const next_empty_slot = *(u32*)&operator[]( push_pointer_value );
			push_pointer_later_value = interlocked_compare_exchange( m_push_pointer, next_empty_slot, push_pointer_value );
		}
		while ( ( push_pointer_later_value != null_value ) && ( push_pointer_later_value != push_pointer_value ) );

		if ( push_pointer_later_value != null_value )
		{
			T* const result = &operator[]( push_pointer_later_value );
			ASSERT( contains_pointer( result ) );

			return result;
		}
	}

	u32 const new_last_pointer = interlocked_inc( m_last_pointer );

	T* const result = &operator[]( new_last_pointer - 1 );
	ASSERT( contains_pointer( result ) );

	u32 const offset = (u32)( (pointer)result - (pointer)m_data );

	bool const first_on_new_page = ( offset % PageSize ) < sizeof(T);

	u32 const expected_page_offset = offset / PageSize + !first_on_new_page;

	SPIN_LOCK( m_page_pointer < expected_page_offset );

	if ( first_on_new_page )
	{
		virtual_allocator( ).commit( (pointer)m_data + PageSize * expected_page_offset, PageSize );
		++m_page_pointer;
	}

	return result;
}

template<typename T, u32 MemorySize, u32 PageSize>
void sys::mt_dynamic_pool<T, MemorySize, PageSize>::deallocate( T* const p )
{
	u32 const index = index_of( p );
	u32* const next_pointer = (u32*)p;

	u32 push_pointer_value;
	u32 push_pointer_later_value = m_push_pointer;

	do
	{
		push_pointer_value = push_pointer_later_value;
		*next_pointer = push_pointer_value;
		push_pointer_later_value = interlocked_compare_exchange( m_push_pointer, index, push_pointer_value );
	}
	while ( push_pointer_value != push_pointer_later_value );
}

template<typename T, u32 MemorySize, u32 PageSize>
T& sys::mt_dynamic_pool<T, MemorySize, PageSize>::operator[]( u32 const index ) const
{
	T* const result = m_data + index;
	ASSERT( contains_pointer( result ) );

	return *result;
}

template<typename T, u32 MemorySize, u32 PageSize>
u32 sys::mt_dynamic_pool<T, MemorySize, PageSize>::index_of( T* const p ) const
{
	ASSERT( contains_pointer( p ) );
	ASSERT( ( (uptr)p - (uptr)m_data ) % sizeof(T) == 0 );

	return (u32)( p - m_data );
}

template<typename T, u32 MemorySize, u32 PageSize>
bool sys::mt_dynamic_pool<T, MemorySize, PageSize>::contains_pointer( pointer const p ) const
{
	return ( p >= (pointer)m_data ) && ( p + sizeof(T) <= (pointer)m_data + MemorySize );
}

#endif // #ifndef GUARD_CORE_MT_POOL_INLINE_H_INCLUDED
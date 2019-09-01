#ifndef GUARD_CORE_POOL_ALLOCATOR_INLINE_H_INCLUDED
#define GUARD_CORE_POOL_ALLOCATOR_INLINE_H_INCLUDED

#include <macros.h>

template<uptr ElemSize>
void lib::pool_allocator<ElemSize>::create( pointer const memory, uptr const memory_size )
{
	static_assert( ElemSize >= sizeof(pointer), "too small element size" );

	m_data = memory ? memory : virtual_allocator( ).allocate( memory_size );
	m_size = memory_size;

	clear( );
}

template<uptr ElemSize>
void lib::pool_allocator<ElemSize>::destroy( )
{
	virtual_allocator( ).deallocate( m_data );
}

template<uptr ElemSize>
pointer lib::pool_allocator<ElemSize>::data( ) const
{
	return m_data;
}

template<uptr ElemSize>
uptr lib::pool_allocator<ElemSize>::size( ) const
{
	return m_size;
}

template<uptr ElemSize>
void lib::pool_allocator<ElemSize>::clear( )
{
	m_push_pointer = nullptr;
	m_last_pointer = m_data;
}

template<uptr ElemSize>
pointer lib::pool_allocator<ElemSize>::operator[]( uptr const index ) const
{
	pointer const result = m_data + index * ElemSize;

	// Note: this check does not guaranties safety.
	ASSERT_CMP( result, <, m_last_pointer );

	return result;
}

template<uptr ElemSize>
uptr lib::pool_allocator<ElemSize>::index_of( pointer const p ) const
{
	// Note: this check does not guaranties safety.
	ASSERT_CMP( p, >=, m_data );
	ASSERT_CMP( p, <=, m_last_pointer );

	uptr const result = (memory_block<ElemSize>*)p - (memory_block<ElemSize>*)m_data;

	return result;
}

template<uptr ElemSize>
bool lib::pool_allocator<ElemSize>::contains( pointer const p ) const
{
	return ( p >= m_data ) && ( p < m_last_pointer );
}

template<uptr ElemSize>
void lib::pool_allocator<ElemSize>::allocate_from_begin( uptr const size )
{
	ASSERT				( m_push_pointer == nullptr );
	ASSERT				( m_last_pointer == m_data );

	ASSERT				( size <= m_size );
	
	m_last_pointer		= m_data + size;
}

template<uptr ElemSize>
pointer lib::pool_allocator<ElemSize>::allocate( uptr const size )
{
	ASSERT_CMP			( size, <=, ElemSize );

	if ( m_push_pointer == nullptr )
	{
		ASSERT_CMP		( m_last_pointer + ElemSize, <=, m_data + m_size ); // pool is full
		pointer result	= m_last_pointer;
		m_last_pointer	+= ElemSize;
		return result;
	}
	else
	{
		pointer result	= m_push_pointer;
		m_push_pointer	= *(pointer*)m_push_pointer;
		return result;
	}
}

template<uptr ElemSize>
void lib::pool_allocator<ElemSize>::deallocate( pointer const p )
{
	ASSERT			( p );
	*(pointer*)p	= m_push_pointer;
	m_push_pointer	= p;
}


template<uptr ElemSize>
void lib::dynamic_pool_allocator<ElemSize>::create( pointer const memory, uptr const memory_size, uptr const initial_size )
{
	static_assert( ElemSize >= sizeof(pointer), "too small element size" );

	ASSERT( initial_size % Memory_Page_Size == 0, "incorrect page size" );

	m_data = memory ? memory : virtual_allocator( ).reserve( nullptr, memory_size );
	m_size = memory_size;
	m_page_pointer = m_data + initial_size;

	virtual_allocator( ).commit( m_data, initial_size );

	clear( );
}

template<uptr ElemSize>
void lib::dynamic_pool_allocator<ElemSize>::destroy( )
{
	virtual_allocator( ).release( m_data );
}

template<uptr ElemSize>
pointer lib::dynamic_pool_allocator<ElemSize>::data( ) const
{
	return m_data;
}

template<uptr ElemSize>
uptr lib::dynamic_pool_allocator<ElemSize>::size( ) const
{
	return m_size;
}

template<uptr ElemSize>
void lib::dynamic_pool_allocator<ElemSize>::clear( )
{
	m_push_pointer = nullptr;
	m_last_pointer = m_data;
}

template<uptr ElemSize>
pointer lib::dynamic_pool_allocator<ElemSize>::operator[]( uptr index ) const
{
	pointer const result = m_data + index * ElemSize;

	// Note: this check does not guaranties safety.
	ASSERT_CMP( result, <, m_last_pointer );

	return result;
}

template<uptr ElemSize>
uptr lib::dynamic_pool_allocator<ElemSize>::index_of( pointer const p ) const
{
	// Note: this check does not guaranties safety.
	ASSERT_CMP( p, >=, m_data );
	ASSERT_CMP( p, <=, m_last_pointer );

	uptr const result = (memory_block<ElemSize>*)p - (memory_block<ElemSize>*)m_data;

	return result;
}

template<uptr ElemSize>
bool lib::dynamic_pool_allocator<ElemSize>::contains( pointer const p ) const
{
	return ( p >= m_data ) && ( p < m_last_pointer );
}

template<uptr ElemSize>
void lib::dynamic_pool_allocator<ElemSize>::allocate_from_begin( uptr const size )
{
	ASSERT				( m_push_pointer == nullptr );
	ASSERT				( m_last_pointer == m_data );

	ASSERT				( size < m_size );
	
	m_last_pointer		= m_data + size;

	pointer const p		= m_page_pointer;

	while ( m_page_pointer < m_last_pointer )
	{
		m_page_pointer	+= m_page_pointer - m_data;
	}

	if ( m_page_pointer != p )
		virtual_allocator( ).commit( p, m_page_pointer - p );
}

template<uptr ElemSize>
pointer lib::dynamic_pool_allocator<ElemSize>::allocate( uptr const size )
{
	ASSERT_CMP( size, <=, ElemSize );

	if ( m_push_pointer == nullptr )
	{
		if ( m_last_pointer + size > m_page_pointer )
		{
			uptr const committed_size	= m_page_pointer - m_data;
			ASSERT_CMP					( m_page_pointer + committed_size, <=, m_data + m_size );
			virtual_allocator( ).commit	( m_page_pointer, committed_size );
			m_page_pointer				+= committed_size;
		}

		pointer result	= m_last_pointer;
		m_last_pointer	+= ElemSize;
		return result;
	}
	else
	{
		pointer result	= m_push_pointer;
		m_push_pointer	= *(pointer*)m_push_pointer;
		return result;
	}
}

template<uptr ElemSize>
void lib::dynamic_pool_allocator<ElemSize>::deallocate( pointer const p )
{
	ASSERT				( p );
	*(pointer*)p		= m_push_pointer;
	m_push_pointer		= p;
}

#endif // #ifndef GUARD_CORE_POOL_ALLOCATOR_INLINE_H_INCLUDED
#ifndef GUARD_CORE_POOL_INLINE_H_INCLUDED
#define GUARD_CORE_POOL_INLINE_H_INCLUDED

#include <macros.h>

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::create( )
{
	create( virtual_allocator( ).commit( nullptr, page_size ) );
}

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::create( pointer memory )
{
	static_assert( ElemSize >= sizeof(pointer), "too small element size" );
	static_assert( ElemSize <= page_size, "too big element size" );
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::destroy( )
{
	virtual_allocator( ).release( m_data );
	m_data = nullptr;
}

template<uptr ElemSize, uptr PageSize>
pointer pool<ElemSize, PageSize>::data( ) const
{
	return m_data;
}

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::clear( )
{
	m_push_pointer = nullptr;
	m_last_pointer = m_data;
}

template<uptr ElemSize, uptr PageSize>
memory_block<ElemSize>& pool<ElemSize, PageSize>::operator[]( uptr const index ) const
{
	memory_block<ElemSize>* const result = (memory_block<ElemSize>*)m_data + index;

	// Note: this check does not guaranties safety.
	ASSERT_CMP( result, <, m_last_pointer );

	return *result;
}

template<uptr ElemSize, uptr PageSize>
uptr pool<ElemSize, PageSize>::index_of( pointer const p ) const
{
	// Note: this check does not guaranties safety.
	ASSERT_CMP( p, >=, m_data );
	ASSERT_CMP( p, <=, m_last_pointer );

	uptr const result = (memory_block<ElemSize>*)p - (memory_block<ElemSize>*)m_data;

	return result;
}

template<uptr ElemSize, uptr PageSize>
pointer pool<ElemSize, PageSize>::allocate( uptr size )
{
	ASSERT_CMP			( size, <=, ElemSize );

	if ( m_push_pointer == nullptr )
	{
		ASSERT_CMP		( m_last_pointer + ElemSize, <=, m_data + page_size ); // pool is full
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

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::deallocate( pointer p )
{
	ASSERT			( p );
	*(pointer*)p	= m_push_pointer;
	m_push_pointer	= p;
}


template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::create( )
{
	create( virtual_allocator( ).reserve( nullptr, page_size * PageMaxCount ) );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::create( pointer memory )
{
	static_assert( ElemSize >= sizeof(pointer), "too small element size" );
	static_assert( ElemSize <= page_size * PageMaxCount, "too big element size" );
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::destroy( )
{
	virtual_allocator( ).release( m_data );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
pointer dynamic_pool<ElemSize, PageSize, PageMaxCount>::data( ) const
{
	return m_data;
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::clear( )
{
	m_page_pointer = m_data - page_size;
	m_push_pointer = nullptr;
	m_last_pointer = m_data;
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
memory_block<ElemSize>& dynamic_pool<ElemSize, PageSize, PageMaxCount>::operator[]( uptr index ) const
{
	memory_block<ElemSize>* const result = (memory_block<ElemSize>*)m_data + index;

	// Note: this check does not guaranties safety.
	ASSERT_CMP( result, <, m_last_pointer );

	return *result;
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
uptr dynamic_pool<ElemSize, PageSize, PageMaxCount>::index_of( pointer const p ) const
{
	// Note: this check does not guaranties safety.
	ASSERT_CMP( p, >=, m_data );
	ASSERT_CMP( p, <=, m_last_pointer );

	uptr const result = (memory_block<ElemSize>*)p - (memory_block<ElemSize>*)m_data;

	return result;
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
pointer dynamic_pool<ElemSize, PageSize, PageMaxCount>::allocate( uptr size )
{
	ASSERT_CMP( size, <=, ElemSize );

	if ( m_push_pointer == nullptr )
	{
		if ( m_last_pointer + size > m_page_pointer + page_size )
		{
			m_page_pointer				+= page_size;
			ASSERT_CMP					( m_page_pointer, <, m_data + page_size * PageMaxCount );
			virtual_allocator( ).commit	( m_page_pointer, page_size );
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

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::deallocate( pointer p )
{
	ASSERT				( p );
	*(pointer*)p		= m_push_pointer;
	m_push_pointer		= p;
}

#endif // #ifndef GUARD_CORE_POOL_INLINE_H_INCLUDED
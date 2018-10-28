#ifndef __core_pool_inline_h_included_
#define __core_pool_inline_h_included_

#include <macros.h>

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::create( )
{
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = virtual_mem_allocator( ).commit( nullptr, page_size );
	clear( );
}

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::create( pointer memory )
{
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::destroy( )
{
	virtual_mem_allocator( ).release( m_data );
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
memory_block<ElemSize>& pool<ElemSize, PageSize>::operator[]( uptr index ) const
{
	memory_block<ElemSize>* const result = (memory_block<ElemSize>*)m_data + index;

	// Note: this check does not guaranties safety.
	ASSERT( result < m_last_pointer );

	return *result;
}

template<uptr ElemSize, uptr PageSize>
pointer pool<ElemSize, PageSize>::allocate( uptr size )
{
	ASSERT				( size <= ElemSize );

	if ( m_push_pointer == nullptr )
	{
		ASSERT			( m_last_pointer + ElemSize <= m_data + page_size ); // pool is full
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
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = virtual_mem_allocator( ).reserve( nullptr, page_size * PageMaxCount );
	clear( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::create( pointer memory )
{
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::destroy( )
{
	virtual_mem_allocator( ).release( m_data );
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
	ASSERT( result < m_last_pointer );

	return *result;
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
pointer dynamic_pool<ElemSize, PageSize, PageMaxCount>::allocate( uptr size )
{
	ASSERT( size <= ElemSize );

	if ( m_push_pointer == nullptr )
	{
		if ( m_last_pointer + size > m_page_pointer + page_size )
		{
			m_page_pointer					+= page_size;
			ASSERT							( m_page_pointer < m_data + page_size * PageMaxCount );
			virtual_mem_allocator( ).commit	( m_page_pointer, page_size );
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

#endif // #ifndef __core_pool_inline_h_included_
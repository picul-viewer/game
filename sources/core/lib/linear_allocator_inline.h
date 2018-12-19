#ifndef __core_linear_allocator_inline_h_included_
#define __core_linear_allocator_inline_h_included_

#include <macros.h>

template<uptr MemorySize>
void linear_allocator<MemorySize>::create( )
{
	m_data = virtual_allocator( ).commit( nullptr, memory_size );
	clear( );
}

template<uptr MemorySize>
void linear_allocator<MemorySize>::destroy( )
{
	virtual_allocator( ).release( m_data );
	m_data = nullptr;
}

template<uptr MemorySize>
pointer linear_allocator<MemorySize>::data( ) const
{
	return m_data;
}

template<uptr MemorySize>
pointer linear_allocator<MemorySize>::data_end( ) const
{
	return m_last_pointer;
}

template<uptr MemorySize>
void linear_allocator<MemorySize>::clear( )
{
	m_last_pointer = m_data;
}

template<uptr MemorySize>
pointer linear_allocator<MemorySize>::allocate( uptr size )
{
	ASSERT					( m_last_pointer + size <= m_data + MemorySize ); // pool is full
	pointer const result	= m_last_pointer;
	m_last_pointer			+= size;
	return result;
}

template<uptr MemorySize>
void linear_allocator<MemorySize>::shrink( uptr const shrink_size )
{
	ASSERT					( (uptr)( m_last_pointer - m_data ) >= shrink_size );
	m_last_pointer			-= shrink_size;
}


template<uptr PageSize, uptr PageMaxCount>
void dynamic_linear_allocator<PageSize, PageMaxCount>::create( )
{
	m_data = virtual_allocator( ).reserve( nullptr, page_size * PageMaxCount );
	clear( );
}

template<uptr PageSize, uptr PageMaxCount>
void dynamic_linear_allocator<PageSize, PageMaxCount>::destroy( )
{
	virtual_allocator( ).release( m_data );
}

template<uptr PageSize, uptr PageMaxCount>
pointer dynamic_linear_allocator<PageSize, PageMaxCount>::data( ) const
{
	return m_data;
}

template<uptr PageSize, uptr PageMaxCount>
pointer dynamic_linear_allocator<PageSize, PageMaxCount>::data_end( ) const
{
	return m_last_pointer;
}

template<uptr PageSize, uptr PageMaxCount>
void dynamic_linear_allocator<PageSize, PageMaxCount>::clear( )
{
	m_page_pointer = m_data;
	m_last_pointer = m_data;
}

template<uptr PageSize, uptr PageMaxCount>
pointer dynamic_linear_allocator<PageSize, PageMaxCount>::allocate( uptr size )
{
	if ( m_last_pointer + size > m_page_pointer )
	{
		uptr const size_to_commit	= get_aligned_size( m_last_pointer + size - m_page_pointer, page_size );
		ASSERT						( m_page_pointer + size_to_commit < m_data + page_size * PageMaxCount );
		virtual_allocator( ).commit	( m_page_pointer, size_to_commit );
		m_page_pointer				+= size_to_commit;
	}

	pointer const result	= m_last_pointer;
	m_last_pointer			+= size;
	return result;
}

template<uptr PageSize, uptr PageMaxCount>
void dynamic_linear_allocator<PageSize, PageMaxCount>::shrink( uptr const shrink_size )
{
	ASSERT					( (uptr)( m_last_pointer - m_data ) >= shrink_size );
	m_last_pointer			-= shrink_size;
}

#endif // #ifndef __core_linear_allocator_inline_h_included_
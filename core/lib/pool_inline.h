#ifndef __core_pool_inline_h_included_
#define __core_pool_inline_h_included_

#include <macros.h>

template<uptr MemorySize>
struct pool_page_size_helper
{
	static const uptr value = ( MemorySize + 4095u ) & ( ~4095u );
};

template<uptr ElemSize, uptr PageSize>
pool<ElemSize, PageSize>::pool( )
{
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = virtual_mem_allocator( ).commit( nullptr, pool_page_size_helper<PageSize>::value );
	clear( );
}

template<uptr ElemSize, uptr PageSize>
pool<ElemSize, PageSize>::pool( pointer memory )
{
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize>
pool<ElemSize, PageSize>::~pool( )
{
	destroy( );
}

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::destroy( )
{
	virtual_mem_allocator( ).release( m_data, pool_page_size_helper<PageSize>::value );
	m_data = nullptr;
}

template<uptr ElemSize, uptr PageSize>
void pool<ElemSize, PageSize>::clear( )
{
	m_push_pointer = nullptr;
	m_last_pointer = m_data;
}

template<uptr ElemSize, uptr PageSize>
pointer pool<ElemSize, PageSize>::allocate( uptr size )
{
	ASSERT				( size <= ElemSize );

	if ( m_push_pointer == nullptr )
	{
		ASSERT			( m_last_pointer + ElemSize <= m_data + pool_page_size_helper<PageSize>::value ); // pool is full
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


template<uptr ElemSize, uptr PageSize>
allocation_pool<ElemSize, PageSize>::allocation_pool( )
{
	m_data = virtual_mem_allocator( ).commit( nullptr, pool_page_size_helper<PageSize>::value );
	clear( );
}

template<uptr ElemSize, uptr PageSize>
allocation_pool<ElemSize, PageSize>::allocation_pool( pointer memory )
{
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize>
allocation_pool<ElemSize, PageSize>::~allocation_pool( )
{
	destroy( );
}

template<uptr ElemSize, uptr PageSize>
void allocation_pool<ElemSize, PageSize>::destroy( )
{
	virtual_mem_allocator( ).release( m_data, pool_page_size_helper<PageSize>::value );
	m_data = nullptr;
}

template<uptr ElemSize, uptr PageSize>
void allocation_pool<ElemSize, PageSize>::clear( )
{
	m_last_pointer = m_data;
}

template<uptr ElemSize, uptr PageSize>
pointer allocation_pool<ElemSize, PageSize>::allocate( uptr size )
{
	ASSERT			( size <= ElemSize );
	ASSERT			( m_last_pointer + ElemSize <= m_data + pool_page_size_helper<PageSize>::value ); // pool is full
	pointer result	= m_last_pointer;
	m_last_pointer	+= ElemSize;
	return			result;
}


template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
dynamic_pool<ElemSize, PageSize, PageMaxCount>::dynamic_pool( )
{
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = virtual_mem_allocator( ).reserve( nullptr, pool_page_size_helper<PageSize>::value * PageMaxCount );
	clear( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
dynamic_pool<ElemSize, PageSize, PageMaxCount>::dynamic_pool( pointer memory )
{
	ASSERT( ElemSize >= sizeof(pointer) );
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
dynamic_pool<ElemSize, PageSize, PageMaxCount>::~dynamic_pool( )
{
	destroy( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::destroy( )
{
	virtual_mem_allocator( ).release( m_data, pool_page_size_helper<PageSize>::value * PageMaxCount );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_pool<ElemSize, PageSize, PageMaxCount>::clear( )
{
	m_page_pointer = m_data - pool_page_size_helper<PageSize>::value;
	m_push_pointer = nullptr;
	m_last_pointer = m_data;
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
pointer dynamic_pool<ElemSize, PageSize, PageMaxCount>::allocate( uptr size )
{
	ASSERT( size <= ElemSize );

	if ( m_push_pointer == nullptr )
	{
		if ( m_last_pointer + size > m_page_pointer + pool_page_size_helper<PageSize>::value )
		{
			m_page_pointer					+= pool_page_size_helper<PageSize>::value;
			ASSERT							( m_page_pointer < m_data + pool_page_size_helper<PageSize>::value * PageMaxCount );
			virtual_mem_allocator( ).commit	( m_page_pointer, pool_page_size_helper<PageSize>::value );
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


template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
dynamic_allocation_pool<ElemSize, PageSize, PageMaxCount>::dynamic_allocation_pool( )
{
	m_data = virtual_mem_allocator( ).reserve( nullptr, pool_page_size_helper<PageSize>::value * PageMaxCount );
	clear( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
dynamic_allocation_pool<ElemSize, PageSize, PageMaxCount>::dynamic_allocation_pool( pointer memory )
{
	m_data = memory;
	clear( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
dynamic_allocation_pool<ElemSize, PageSize, PageMaxCount>::~dynamic_allocation_pool( )
{
	destroy( );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_allocation_pool<ElemSize, PageSize, PageMaxCount>::destroy( )
{
	virtual_mem_allocator( ).release( m_data, pool_page_size_helper<PageSize>::value * PageMaxCount );
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
void dynamic_allocation_pool<ElemSize, PageSize, PageMaxCount>::clear( )
{
	m_page_pointer = m_data - pool_page_size_helper<PageSize>::value;
	m_last_pointer = m_data;
}

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
pointer dynamic_allocation_pool<ElemSize, PageSize, PageMaxCount>::allocate( uptr size )
{
	ASSERT( size <= ElemSize );

	if ( m_last_pointer + size > m_page_pointer + pool_page_size_helper<PageSize>::value )
	{
		m_page_pointer					+= pool_page_size_helper<PageSize>::value;
		ASSERT							( m_page_pointer < m_data + pool_page_size_helper<PageSize>::value * PageMaxCount );
		virtual_mem_allocator( ).commit	( m_page_pointer, pool_page_size_helper<PageSize>::value );
	}

	pointer result	= m_last_pointer;
	m_last_pointer	+= ElemSize;
	return result;
}


template<uptr PageSize>
allocation_multipool<PageSize>::allocation_multipool( )
{
	m_data = virtual_mem_allocator( ).commit( nullptr, pool_page_size_helper<PageSize>::value );
	clear( );
}

template<uptr PageSize>
allocation_multipool<PageSize>::allocation_multipool( pointer memory )
{
	m_data = memory;
	clear( );
}

template<uptr PageSize>
allocation_multipool<PageSize>::~allocation_multipool( )
{
	destroy( );
}

template<uptr PageSize>
void allocation_multipool<PageSize>::destroy( )
{
	virtual_mem_allocator( ).release( m_data, pool_page_size_helper<PageSize>::value );
	m_data = nullptr;
}

template<uptr PageSize>
void allocation_multipool<PageSize>::clear( )
{
	m_last_pointer	= m_data;
}

template<uptr PageSize>
pointer allocation_multipool<PageSize>::allocate( uptr size )
{
	ASSERT			( m_last_pointer + size <= m_data + pool_page_size_helper<PageSize>::value ); // pool is full
	pointer result	= m_last_pointer;
	m_last_pointer	+= size;
	return			result;
}


template<uptr PageSize, uptr PageMaxCount>
dynamic_allocation_multipool<PageSize, PageMaxCount>::dynamic_allocation_multipool( )
{
	m_data = virtual_mem_allocator( ).reserve( nullptr, pool_page_size_helper<PageSize>::value * PageMaxCount );
	clear( );
}

template<uptr PageSize, uptr PageMaxCount>
dynamic_allocation_multipool<PageSize, PageMaxCount>::dynamic_allocation_multipool( pointer memory )
{
	m_data = memory;
	clear( );
}

template<uptr PageSize, uptr PageMaxCount>
dynamic_allocation_multipool<PageSize, PageMaxCount>::~dynamic_allocation_multipool( )
{
	destroy( );
}

template<uptr PageSize, uptr PageMaxCount>
void dynamic_allocation_multipool<PageSize, PageMaxCount>::destroy( )
{
	virtual_mem_allocator( ).release( m_data, pool_page_size_helper<PageSize>::value * PageMaxCount );
	m_data = nullptr;
}

template<uptr PageSize, uptr PageMaxCount>
void dynamic_allocation_multipool<PageSize, PageMaxCount>::clear( )
{
	m_page_pointer	= m_data - pool_page_size_helper<PageSize>::value;
	m_last_pointer	= m_data;
}

template<uptr PageSize, uptr PageMaxCount>
pointer dynamic_allocation_multipool<PageSize, PageMaxCount>::allocate( uptr size )
{
	if ( m_last_pointer + size <= m_page_pointer + pool_page_size_helper<PageSize>::value )
	{
		m_page_pointer					+= pool_page_size_helper<PageSize>::value;
		ASSERT							( m_page_pointer < m_data + pool_page_size_helper<PageSize>::value * PageMaxCount );
		virtual_mem_allocator( ).commit	( m_page_pointer, pool_page_size_helper<PageSize>::value );
	}

	pointer result		= m_last_pointer;
	m_last_pointer		+= size;
	return result;
}

#endif // #ifndef __core_pool_inline_h_included_
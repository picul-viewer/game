#ifndef GUARD_CORE_POOL_H_INCLUDED
#define GUARD_CORE_POOL_H_INCLUDED

#include <types.h>
#include "allocator.h"
#include "memory_block.h"

template<uptr ElemSize, uptr PageSize>
struct pool
{
public:
	void create( pointer const in_committed_memory = nullptr );
	void destroy( );

	pointer data( ) const;
	void clear( );

	memory_block<ElemSize>& operator[]( uptr const index ) const;
	uptr index_of( pointer const p ) const;

	pointer allocate( uptr size );
	void deallocate( pointer p );

public:
	enum : uptr {
		page_size = virtual_allocator::memory_size_helper<PageSize>::value,
		memory_size = page_size
	};

protected:
	pointer	m_data;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
struct dynamic_pool
{
public:
	void create( pointer const in_reserved_memory = nullptr );
	void destroy( );

	pointer data( ) const;
	void clear( );
	
	memory_block<ElemSize>& operator[]( uptr const index ) const;
	uptr index_of( pointer const p ) const;

	pointer allocate( uptr size );
	void deallocate( pointer p );

public:
	enum : uptr {
		page_size = virtual_allocator::memory_size_helper<PageSize>::value,
		memory_size = page_size * PageMaxCount
	};

protected:
	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

#include "pool_inline.h"

#endif // #ifndef GUARD_CORE_POOL_H_INCLUDED
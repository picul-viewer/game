#ifndef __core_linear_allocator_h_included_
#define __core_linear_allocator_h_included_

#include <types.h>
#include "allocator.h"

template<uptr MemorySize>
struct linear_allocator
{
public:
	void create( );
	void destroy( );
	
	pointer data( ) const;
	void clear( );

	pointer allocate( uptr size );

protected:
	enum : uptr { memory_size = virtual_mem_allocator::memory_size_helper<MemorySize>::value };

	pointer	m_data;
	pointer	m_last_pointer;

};

template<uptr PageSize, uptr PageMaxCount>
struct dynamic_linear_allocator
{
public:
	void create( );
	void destroy( );
	
	pointer data( ) const;
	void clear( );

	pointer allocate( uptr size );

protected:
	enum : uptr { page_size = virtual_mem_allocator::memory_size_helper<PageSize>::value };

	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_last_pointer;
};

#include "linear_allocator_inline.h"

#endif // #ifndef __core_linear_allocator_h_included_
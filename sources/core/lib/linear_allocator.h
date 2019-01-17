#ifndef __core_linear_allocator_h_included_
#define __core_linear_allocator_h_included_

#include <types.h>
#include "allocator.h"

struct linear_allocator
{
public:
	void create( pointer const memory, uptr const size );
	void destroy( );
	
	pointer data( ) const;
	pointer data_end( ) const;
	void clear( );

	pointer allocate( uptr size );

	void shrink( uptr const shrink_size );

protected:
	pointer	m_data;
	pointer	m_data_end;
	pointer	m_last_pointer;

};

template<uptr PageSize, uptr PageMaxCount>
struct dynamic_linear_allocator
{
public:
	void create( );
	void destroy( );
	
	pointer data( ) const;
	pointer data_end( ) const;
	void clear( );

	pointer allocate( uptr size );
	
	void shrink( uptr const shrink_size );

protected:
	enum : uptr { page_size = virtual_allocator::memory_size_helper<PageSize>::value };

	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_last_pointer;
};

#include "linear_allocator_inline.h"

#endif // #ifndef __core_linear_allocator_h_included_
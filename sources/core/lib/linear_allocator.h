#ifndef GUARD_CORE_LINEAR_ALLOCATOR_H_INCLUDED
#define GUARD_CORE_LINEAR_ALLOCATOR_H_INCLUDED

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

	uptr index_of( pointer const p ) const;

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

	uptr index_of( pointer const p ) const;

protected:
	enum : uptr { page_size = virtual_allocator::memory_size_helper<PageSize>::value };

	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_last_pointer;
};

#include "linear_allocator_inline.h"

#endif // #ifndef GUARD_CORE_LINEAR_ALLOCATOR_H_INCLUDED
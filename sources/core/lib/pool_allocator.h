#ifndef GUARD_CORE_POOL_ALLOCATOR_H_INCLUDED
#define GUARD_CORE_POOL_ALLOCATOR_H_INCLUDED

#include <types.h>
#include "allocator.h"

namespace lib {

template<uptr ElemSize>
struct pool_allocator
{
public:
	void create( pointer const memory, uptr const memory_size );
	void destroy( );

	pointer data( ) const;
	uptr size( ) const;

	void clear( );

	pointer operator[]( uptr const index ) const;
	uptr index_of( pointer const p ) const;

	bool contains( pointer const p ) const;

	void allocate_from_begin( uptr const size );

	pointer allocate( uptr const size = ElemSize );
	void deallocate( pointer const p );

protected:
	pointer	m_data;
	uptr m_size;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

template<uptr ElemSize>
struct dynamic_pool_allocator
{
public:
	void create( pointer const memory, uptr const memory_size, uptr const initial_size );
	void destroy( );

	pointer data( ) const;
	uptr size( ) const;

	void clear( );
	
	pointer operator[]( uptr const index ) const;
	uptr index_of( pointer const p ) const;

	bool contains( pointer const p ) const;

	void allocate_from_begin( uptr const size );

	pointer allocate( uptr const size = ElemSize );
	void deallocate( pointer const p );

protected:
	pointer	m_data;
	uptr m_size;
	pointer m_page_pointer;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

} // namespace lib

#include "pool_allocator_inline.h"

#endif // #ifndef GUARD_CORE_POOL_ALLOCATOR_H_INCLUDED
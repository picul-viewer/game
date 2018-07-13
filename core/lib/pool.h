#ifndef __core_pool_h_included_
#define __core_pool_h_included_

#include <types.h>
#include "allocator.h"

template<uptr ElemSize, uptr PageSize>
struct pool
{
public:
	void create( );
	void destroy( );

	inline pointer get_data( ) { return m_data; }
	void clear( );

	pointer allocate( uptr size );
	void deallocate( pointer p );

protected:
	void create( pointer memory );

	pointer	m_data;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

template<uptr ElemSize, uptr PageSize>
struct allocation_pool
{
public:
	void create( );
	void destroy( );
	
	inline pointer get_data( ) { return m_data; }
	void clear( );

	pointer allocate( uptr size );

protected:
	void create( pointer memory );

	pointer	m_data;
	pointer	m_last_pointer;
};

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
struct dynamic_pool
{
public:
	void create( );
	void destroy( );

	inline pointer get_data( ) { return m_data; }
	void clear( );

	pointer allocate( uptr size );
	void deallocate( pointer p );

protected:
	void create( pointer memory );

	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
struct dynamic_allocation_pool
{
public:
	void create( );
	void destroy( );
	
	inline pointer get_data( ) { return m_data; }
	void clear( );

	pointer allocate( uptr size );

protected:
	void create( pointer memory );

	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_last_pointer;
};

template<uptr PageSize>
struct allocation_multipool
{
public:
	void create( );
	void destroy( );
	
	inline pointer get_data( ) { return m_data; }
	void clear( );

	pointer allocate( uptr size );

protected:
	void create( pointer memory );

	pointer	m_data;
	pointer	m_last_pointer;
};

template<uptr PageSize, uptr PageMaxCount>
struct dynamic_allocation_multipool
{
public:
	void create( );
	void destroy( );
	
	inline pointer get_data( ) { return m_data; }
	void clear( );

	pointer allocate( uptr size );

protected:
	void create( pointer memory );

	pointer	m_data;
	pointer	m_page_pointer;
	pointer	m_last_pointer;
};

#include "pool_inline.h"

#endif // #ifndef __core_pool_h_included_
#ifndef __core_pool_h_included_
#define __core_pool_h_included_

#include <core/types.h>
#include "allocator.h"

template<uptr ElemSize, uptr PageSize>
struct pool
{
public:
	pool( );
	~pool( );

	void destroy( );

	void clear( );

	pointer allocate( uptr size );
	void deallocate( pointer p );

	pointer data( ) const;

protected:
	pool( pointer memory );

	pointer	m_data;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

template<uptr ElemSize, uptr PageSize>
struct allocation_pool
{
public:
	allocation_pool( );
	~allocation_pool( );

	void destroy( );
	
	void clear( );

	pointer allocate( uptr size );

	pointer data( ) const;

protected:
	allocation_pool( pointer memory );

	pointer	m_data;
	pointer	m_last_pointer;
};

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
struct dynamic_pool
{
public:
	dynamic_pool( );
	~dynamic_pool( );

	void destroy( );
	
	void clear( );

	pointer allocate( uptr size );
	void deallocate( pointer p );

	pointer data( ) const;

protected:
	dynamic_pool( pointer memory );

	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_push_pointer;
	pointer	m_last_pointer;
};

template<uptr ElemSize, uptr PageSize, uptr PageMaxCount>
struct dynamic_allocation_pool
{
public:
	dynamic_allocation_pool( );
	~dynamic_allocation_pool( );

	void destroy( );
	
	void clear( );

	pointer allocate( uptr size );

	pointer data( ) const;

protected:
	dynamic_allocation_pool( pointer memory );

	pointer	m_data;
	pointer m_page_pointer;
	pointer	m_last_pointer;
};

template<uptr PageSize>
struct allocation_multipool
{
public:
	allocation_multipool( );
	~allocation_multipool( );

	void destroy( );
	
	void clear( );

	pointer allocate( uptr size );

	pointer data( ) const;

protected:
	allocation_multipool( pointer memory );

	pointer	m_data;
	pointer	m_last_pointer;
};

template<uptr PageSize, uptr PageMaxCount>
struct dynamic_allocation_multipool
{
public:
	dynamic_allocation_multipool( );
	~dynamic_allocation_multipool( );

	void destroy( );
	
	void clear( );

	pointer allocate( uptr size );

	pointer data( ) const;

protected:
	dynamic_allocation_multipool( pointer memory );

	pointer	m_data;
	pointer	m_page_pointer;
	pointer	m_last_pointer;
};

#include "pool_inline.h"

#endif // #ifndef __core_pool_h_included_
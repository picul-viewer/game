#ifndef __core_heap_h_included_
#define __core_heap_h_included_

#include <core/types.h>
#include <core/structs/allocator.h>

template<uptr ElemSize, uptr BatchSize, typename Allocator = mem_allocator>
struct array_heap
{
public:
	array_heap( Allocator* allocator = nullptr );
	~array_heap( );

	void destroy( );

	pointer allocate( uptr size );
	void deallocate( pointer p );

protected:
	struct memory_batch
	{
		byte data[ElemSize * BatchSize];
	};

	memory_batch*	m_data;
	pointer			m_push_pointer;
	pointer			m_last_pointer;
	Allocator*		m_allocator;
};

template<uptr ElemSize, uptr BatchSize, typename Allocator = mem_allocator>
struct array_allocation_heap
{
public:
	array_allocation_heap( Allocator* allocator = nullptr );
	~array_allocation_heap( );

	void destroy( );

	pointer allocate( uptr size );

protected:
	struct memory_batch
	{
		byte data[ElemSize * BatchSize];
	};

	memory_batch*	m_data;
	pointer			m_push_pointer;
	Allocator*		m_allocator;
};

template<uptr ElemSize, uptr BatchSize, typename Allocator = mem_allocator>
struct dynamic_array_heap
{
public:
	dynamic_array_heap( Allocator* allocator = nullptr );
	~dynamic_array_heap( );

	void destroy( );
	
	pointer allocate( uptr size );
	void deallocate( pointer p );

protected:
	struct memory_batch
	{
		byte			data[ElemSize * BatchSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	pointer			m_push_pointer;
	pointer			m_last_pointer;
	Allocator*		m_allocator;
};

template<uptr ElemSize, uptr BatchSize, typename Allocator = mem_allocator>
struct dynamic_array_allocation_heap
{
public:
	dynamic_array_allocation_heap( Allocator* allocator = nullptr );
	~dynamic_array_allocation_heap( );

	void destroy( );
	
	pointer allocate( uptr size );

protected:
	struct memory_batch
	{
		byte			data[ElemSize * BatchSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	pointer			m_push_pointer;
	Allocator*		m_allocator;
};

template<uptr SizeMin, uptr SizeMax, uptr SizeStep, uptr PageSize, typename Allocator = mem_allocator>
struct dynamic_heap
{
public:
	dynamic_heap( Allocator* allocator = nullptr );
	~dynamic_heap( );

	void destroy( );

	pointer allocate( uptr size );
	void deallocate( pointer p, uptr size );
	
protected:
	enum { push_pointers_count = ( SizeMax - SizeMin ) / SizeStep };

	struct memory_batch
	{
		byte			data[PageSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	pointer			m_push_pointers[push_pointers_count];
	pointer			m_last_pointer;
	Allocator*		m_allocator;
};

template<uptr PageSize, typename Allocator = mem_allocator>
struct dynamic_allocation_heap
{
public:
	dynamic_allocation_heap( Allocator* allocator = nullptr );
	~dynamic_allocation_heap( );

	void destroy( );

	pointer allocate( uptr size );

protected:
	struct memory_batch
	{
		byte			data[PageSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	pointer			m_last_pointer;
	Allocator*		m_allocator;
};

template<u32 PageSize, typename Allocator, typename ... TList>
struct multi_heap;

template<u32 PageSize, typename Allocator>
struct multi_heap<PageSize, Allocator>
{
public:
	void destroy( );

	template<typename U>
	U* allocate( );
	template<typename U>
	void allocate( U*& pointer );
	template<typename U>
	void deallocate( U* pointer );
};

template<u32 PageSize, typename Allocator, typename T, typename ... TList>
struct multi_heap<PageSize, Allocator, T, TList ...> :
	protected multi_heap<PageSize, Allocator, TList ...>,
	protected array_heap<sizeof(T), PageSize / sizeof(T), Allocator>
{
public:
	multi_heap( );
	~multi_heap( );

	void destroy( );
	
	template<typename U>
	U* allocate( )
	{
		return multi_heap<Allocator, PageSize, TList ...>::allocate<U>( );
	}

	template<>
	T* allocate( )
	{
		return array_heap<T, Allocator, PageSize / sizeof(T)>::allocate( );
	}

	template<typename U>
	void allocate( U*& pointer )
	{
		multi_heap<Allocator, PageSize, TList ...>::allocate<U>( pointer );
	}

	template<>
	void allocate( T*& pointer )
	{
		pointer = array_heap<T, Allocator, PageSize / sizeof(T)>::allocate( );
	}

	template<typename U>
	void deallocate( U* pointer )
	{
		multi_heap<Allocator, PageSize, TList ...>::deallocate( pointer );
	}

	template<>
	void deallocate( T* pointer )
	{
		array_heap<T, Allocator, PageSize / sizeof(T)>::deallocate( pointer );
	}
};

template<typename T, u32 PageSize, u32 PagesCount, typename Allocator = mem_allocator>
class linear_array_heap
{
public:
	linear_array_heap( );
	~linear_array_heap( );
	
	void destroy( );

	u32 allocate( );
	void deallocate( u32 index );

protected:
	typedef typed_memory_helper<sizeof(T), T> Thelp;

	Thelp*	m_data[PagesCount];
	u32		m_last_index;
	u32		m_push_index;
};

#include "heap_inline.h"

#endif // #ifndef __core_heap_h_included_

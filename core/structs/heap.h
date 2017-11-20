#ifndef __core_heap_h_included_
#define __core_heap_h_included_

#include <core/types.h>
#include <core/structs/allocator.h>

template<typename T, u32 BatchSize, typename Allocator = mem_allocator>
struct array_heap
{
public:
	array_heap( );
	~array_heap( );

	void destroy( );

	T* allocate( );
	void deallocate( T* pointer );

protected:
	typedef typed_memory_helper<sizeof(T), T> Thelp;

	struct memory_batch
	{
		Thelp data[BatchSize];
	};

	memory_batch*	m_data;
	T*				m_push_pointer;
	T*				m_last_pointer;
};

template<typename T, u32 BatchSize, typename Allocator = mem_allocator>
struct array_allocation_heap
{
public:
	array_allocation_heap();
	~array_allocation_heap();

	void destroy();

	T* allocate();

protected:
	typedef typed_memory_helper<sizeof(T), T> Thelp;

	struct memory_batch
	{
		Thelp data[BatchSize];
	};

	memory_batch*	m_data;
	T*				m_push_pointer;
};

template<typename T, u32 BatchSize, typename Allocator = mem_allocator>
struct dynamic_array_heap
{
public:
	dynamic_array_heap( );
	~dynamic_array_heap( );

	void destroy( );

	T* allocate( );
	void deallocate( T* pointer );

protected:
	typedef typed_memory_helper<sizeof(T), T> Thelp;

	struct memory_batch
	{
		Thelp			data[BatchSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	T*				m_push_pointer;
	T*				m_last_pointer;
};

template<typename T, u32 BatchSize, typename Allocator = mem_allocator>
struct dynamic_array_allocation_heap
{
public:
	dynamic_array_allocation_heap( );
	~dynamic_array_allocation_heap( );

	void destroy( );

	T* allocate( );

protected:
	typedef typed_memory_helper<sizeof(T), T> Thelp;

	struct memory_batch
	{
		Thelp			data[BatchSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	T*				m_push_pointer;
};

template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator = mem_allocator>
struct dynamic_heap
{
public:
	dynamic_heap( );
	~dynamic_heap( );

	void destroy( );

	template<typename T>
	T* allocate( );
	template<typename T>
	void allocate( T*& pointer );
	template<typename T>
	void deallocate( T* pointer );
	
	void* allocate_size( u32 size );
	void deallocate_size( void* pointer, u32 size );

protected:
	enum { push_pointers_count = ( SizeMax - SizeMin ) / SizeStep };

	struct memory_batch
	{
		char			data[PageSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	char*			m_push_pointers[push_pointers_count];
	char*			m_last_pointer;
};

template<u32 PageSize, typename Allocator = mem_allocator>
struct dynamic_allocation_heap
{
public:
	dynamic_allocation_heap( );
	~dynamic_allocation_heap( );

	void destroy( );

	template<typename T>
	T* allocate( );
	template<typename T>
	void allocate( T*& pointer );
	
	void* allocate_size( uptr size );

protected:
	struct memory_batch
	{
		char			data[PageSize];
		memory_batch*	prev;
	};

	memory_batch*	m_data;
	char*			m_last_pointer;
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
	protected array_heap<T, PageSize / sizeof(T), Allocator>
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

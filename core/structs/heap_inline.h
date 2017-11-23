#ifndef __core_heap_inline_h_included_
#define __core_heap_inline_h_included_

#include <core/macros.h>

template<typename T, u32 BatchSize, typename Allocator>
array_heap<T, BatchSize, Allocator>::array_heap( )
{
	ASSERT			( sizeof(T) >= sizeof(T*) );
	m_data			= (memory_batch*)Allocator::allocate( sizeof(memory_batch) );
	m_push_pointer	= nullptr;
	m_last_pointer	= (T*)m_data->data;
}

template<typename T, u32 BatchSize, typename Allocator>
array_heap<T, BatchSize, Allocator>::~array_heap( )
{
	destroy( );
}

template<typename T, u32 BatchSize, typename Allocator>
void array_heap<T, BatchSize, Allocator>::destroy( )
{
	Allocator::deallocate( m_data );
	m_data = nullptr;
}

template<typename T, u32 BatchSize, typename Allocator>
T* array_heap<T, BatchSize, Allocator>::allocate( )
{
	T* result = nullptr;
	if ( m_push_pointer == nullptr )
	{
		ASSERT			( m_last_pointer < (T*)m_data->data + BatchSize ); // heap is full
		result			= m_last_pointer;
		++m_last_pointer;
	}
	else
	{
		result			= m_push_pointer;
		m_push_pointer	= *(T**)m_push_pointer;
	}
	return result;
}

template<typename T, u32 BatchSize, typename Allocator>
void array_heap<T, BatchSize, Allocator>::deallocate( T* pointer )
{
	ASSERT			( pointer );
	*(T**)pointer	= m_push_pointer;
	m_push_pointer	= pointer;
}


template<typename T, u32 BatchSize, typename Allocator>
array_allocation_heap<T, BatchSize, Allocator>::array_allocation_heap( )
{
	m_data			= (memory_batch*)Allocator::allocate( sizeof(memory_batch) );
	m_push_pointer	= (T*)m_data->data;
}

template<typename T, u32 BatchSize, typename Allocator>
array_allocation_heap<T, BatchSize, Allocator>::~array_allocation_heap()
{
	destroy();
}

template<typename T, u32 BatchSize, typename Allocator>
void array_allocation_heap<T, BatchSize, Allocator>::destroy()
{
	Allocator::deallocate(m_data);
	m_data = nullptr;
}

template<typename T, u32 BatchSize, typename Allocator>
T* array_allocation_heap<T, BatchSize, Allocator>::allocate()
{
	ASSERT		( m_push_pointer < (T*)m_data->data + BatchSize ); // heap is full
	T* result	= m_push_pointer;
	++m_push_pointer;
	return		result;
}


template<typename T, u32 BatchSize, typename Allocator>
dynamic_array_heap<T, BatchSize, Allocator>::dynamic_array_heap( )
{
	ASSERT			( sizeof(T) >= sizeof(T*) );
	m_data			= nullptr;
	m_data->prev	= nullptr;
	m_push_pointer	= nullptr;
	m_last_pointer	= (T*)m_data->data + BatchSize - 1;
}

template<typename T, u32 BatchSize, typename Allocator>
dynamic_array_heap<T, BatchSize, Allocator>::~dynamic_array_heap( )
{
	destroy( );
}

template<typename T, u32 BatchSize, typename Allocator>
void dynamic_array_heap<T, BatchSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		Allocator::deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<typename T, u32 BatchSize, typename Allocator>
T* dynamic_array_heap<T, BatchSize, Allocator>::allocate( )
{
	if ( m_push_pointer == nullptr )
	{
		if ( m_last_pointer != (T*)m_data->data + BatchSize - 1 )
		{
			++m_last_pointer;
			return m_last_pointer;
		}
		else
		{
			memory_batch* mem	= m_data;
			m_data				= (memory_batch*)Allocator::allocate( sizeof(memory_batch) );
			m_data->prev		= mem;
			m_last_pointer		= (T*)m_data->data;
			return m_last_pointer;
		}
	}
	else
	{
		T* result		= m_push_pointer;
		m_push_pointer	= *(T**)m_push_pointer;
		return result;
	}
}

template<typename T, u32 BatchSize, typename Allocator>
void dynamic_array_heap<T, BatchSize, Allocator>::deallocate( T* pointer )
{
	ASSERT			( pointer );
	*(T**)pointer	= m_push_pointer;
	m_push_pointer	= pointer;
}


template<typename T, u32 BatchSize, typename Allocator>
dynamic_array_allocation_heap<T, BatchSize, Allocator>::dynamic_array_allocation_heap( )
{
	m_data			= nullptr;
	m_data->prev	= nullptr;
	m_push_pointer	= (T*)m_data->data + Batchsize - 1;
}

template<typename T, u32 BatchSize, typename Allocator>
dynamic_array_allocation_heap<T, BatchSize, Allocator>::~dynamic_array_allocation_heap( )
{
	destroy( );
}

template<typename T, u32 BatchSize, typename Allocator>
void dynamic_array_allocation_heap<T, BatchSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		Allocator::deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<typename T, u32 BatchSize, typename Allocator>
T* dynamic_array_allocation_heap<T, BatchSize, Allocator>::allocate( )
{
	if ( m_push_pointer != (T*)m_data->data + BatchSize - 1 )
		++m_push_pointer;
	else
	{
		memory_batch* mem	= m_data;
		m_data				= (memory_batch*)Allocator::allocate( sizeof(memory_batch) );
		m_data->prev		= mem;
		m_push_pointer		= (T*)m_data->data;
	}
	return m_push_pointer;
}


template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::dynamic_heap( )
{
	ASSERT			( SizeMin >= sizeof(char*) );
	ASSERT			( SizeMax <= PageSize );
	ASSERT			( ( SizeMax - SizeMin ) % SizeStep != 0 );
	m_data			= nullptr;
	m_data->prev	= nullptr;
	memset( m_push_pointers, 0, sizeof(char*) * push_pointers_count );
	m_last_pointer	= m_data->data + PageSize;
}

template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::~dynamic_heap( )
{
	destroy( );
}

template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
void dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		Allocator::deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
template<typename T>
T* dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::allocate( )
{
	return (T*)allocate_size( sizeof(T) );
}

template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
template<typename T>
void dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::allocate( T*& pointer )
{
	pointer = (T*)allocate_size( sizeof(T) );
}

template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
void* dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::allocate_size( u32 size )
{
	ASSERT						( size < SizeMin );
	ASSERT						( size > SizeMax );
	ASSERT						( ( size - SizeMin ) % SizeStep );
	u32 const index				= ( size - SizeMin ) / SizeStep;

	if ( m_push_pointers[index] == nullptr )
	{
		if ( m_last_pointer + size <= m_data->data + PageSize )
		{
			void* result		= m_last_pointer;
			m_last_pointer		+= size;
			return result;
		}
		else
		{
			memory_batch* mem	= m_data;
			m_data				= (memory_batch*)Allocator::allocate( sizeof(memory_batch) );
			m_data->prev		= mem;
			m_last_pointer		= m_data->data + size;
			return m_data->data;
		}
	}
	else
	{
		void* result			= m_push_pointers[index];
		m_push_pointers[index]	= *(char**)m_push_pointers[index];
		return result;
	}
}

// NOTE - static type resolve!!
template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
template<typename T>
void dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::deallocate( T* pointer )
{
	deallocate_size( pointer, sizeof(T) );
}

template<u32 SizeMin, u32 SizeMax, u32 SizeStep, u32 PageSize, typename Allocator>
void dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::deallocate_size( void* pointer, u32 size )
{
	ASSERT					( pointer );

	ASSERT					( size < SizeMin );
	ASSERT					( size > SizeMin );
	ASSERT					( ( size - SizeMin ) % SizeStep );
	u32 const index			= ( size - SizeMin ) / SizeStep;

	*(char**)pointer		= m_push_pointers[index];
	m_push_pointer[index]	= pointer;
}


template<u32 PageSize, typename Allocator>
dynamic_allocation_heap<PageSize, Allocator>::dynamic_allocation_heap( )
{
	m_data			= nullptr;
	m_data->prev	= nullptr;
	m_last_pointer	= m_data->data + PageSize;
}

template<u32 PageSize, typename Allocator>
dynamic_allocation_heap<PageSize, Allocator>::~dynamic_allocation_heap( )
{
	destroy( );
}

template<u32 PageSize, typename Allocator>
void dynamic_allocation_heap<PageSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		Allocator::deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<u32 PageSize, typename Allocator>
template<typename T>
T* dynamic_allocation_heap<PageSize, Allocator>::allocate( )
{
	return allocate_size( sizeof(T) );
}

template<u32 PageSize, typename Allocator>
template<typename T>
void dynamic_allocation_heap<PageSize, Allocator>::allocate( T*& pointer )
{
	pointer = allocate_size( sizeof(T) );
}

template<u32 PageSize, typename Allocator>
void* dynamic_allocation_heap<PageSize, Allocator>::allocate_size( uptr size )
{
	if ( m_last_pointer + size <= m_data->data + PageSize )
	{
		void* result		= m_last_pointer;
		m_last_pointer		+= size;
		return result;
	}
	else
	{
		memory_batch* mem	= m_data;
		m_data				= (memory_batch*)Allocator::allocate( sizeof(memory_batch) );
		m_data->prev		= mem;
		m_last_pointer		= m_data->data + size;
		return m_data->data;
	}
}


template<u32 PageSize, typename Allocator>
void multi_heap<PageSize, Allocator>::destroy( )
{ }

template<u32 PageSize, typename Allocator>
template<typename U>
U* multi_heap<PageSize, Allocator>::allocate( )
{
	FATAL_ERROR( "provided type size is not supported by this heap" );
	return nullptr;
}

template<u32 PageSize, typename Allocator>
template<typename U>
void multi_heap<PageSize, Allocator>::allocate( U*& pointer )
{
	FATAL_ERROR( "provided type size is not supported by this heap" );
}

template<u32 PageSize, typename Allocator>
template<typename U>
void multi_heap<PageSize, Allocator>::deallocate( U* pointer )
{
	FATAL_ERROR( "provided type size is not supported by this heap" );
}

template<u32 PageSize, typename Allocator, typename T, typename ... TList>
multi_heap<PageSize, Allocator, T, TList ...>::multi_heap( )
{ }

template<u32 PageSize, typename Allocator, typename T, typename ... TList>
multi_heap<PageSize, Allocator, T, TList ...>::~multi_heap( )
{
	array_heap<T, PageSize / sizeof(T), Allocator>::destroy( );
	multi_heap<PageSize, Allocator, TList ...>::destroy( );
}

template<u32 PageSize, typename Allocator, typename T, typename ... TList>
void multi_heap<PageSize, Allocator, T, TList ...>::destroy( )
{
	array_heap<T, PageSize / sizeof(T), Allocator>::destroy( );
	multi_heap<PageSize, Allocator, TList ...>::destroy( );
}


template<typename T, u32 PageSize, u32 PagesCount, typename Allocator>
linear_array_heap<T, PageSize, PagesCount, Allocator>::linear_array_heap( ) :
	m_last_index	( 0 ),
	m_push_index	( (u32)-1 )
{
	ASSERT( sizeof(T) > sizeof(u32) );
	ASSERT( (u64)PageSize * (u64)PageCount > (u64)(u32)-1 );
}

template<typename T, u32 PageSize, u32 PagesCount, typename Allocator>
linear_array_heap<T, PageSize, PagesCount, Allocator>::~linear_array_heap( )
{
	destroy( );
}

template<typename T, u32 PageSize, u32 PagesCount, typename Allocator>
void linear_array_heap<T, PageSize, PagesCount, Allocator>::destroy( )
{
	for ( u32 i = 0; i < PageCount; ++i )
		if ( m_data[i] )
		{
			Allocator::deallocate( m_data[i] );
			m_data[i] = nullptr;
		}
}

template<typename T, u32 PageSize, u32 PagesCount, typename Allocator>
u32 linear_array_heap<T, PageSize, PagesCount, Allocator>::allocate( )
{
	u32 result			= 0;
	if ( m_push_index == (u32)-1 )
	{
		ASSERT			( m_last_index < PagesCount * PageSize );

		if ( m_last_index % PageSize == 0 )
			m_data[m_last_index / PageSize] = (T*)Allocator::allocate( PageSize * sizeof(T) );

		result			= m_last_index;
		++m_last_index;
	}
	else
	{
		result			= m_push_index;
		m_push_index	= *(u32*)( m_data[m_push_index / PageSize] + m_push_index % PageSize );
	}
	return result;
}

template<typename T, u32 PageSize, u32 PagesCount, typename Allocator>
void linear_array_heap<T, PageSize, PagesCount, Allocator>::deallocate( u32 index )
{
	*(u32*)( m_data[index / PageSize] + index % PageSize )	= m_push_index;
	m_push_index											= index;
}

#endif // #ifndef __core_heap_inline_h_included_

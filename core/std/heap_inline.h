#ifndef __core_heap_inline_h_included_
#define __core_heap_inline_h_included_

#include <core/macros.h>

template<uptr ElemSize, uptr BatchSize, typename Allocator>
array_heap<ElemSize, BatchSize, Allocator>::array_heap( Allocator* allocator )
{
	ASSERT			( ElemSize >= sizeof(pointer) );
	m_allocator		= allocator;
	m_data			= (memory_batch*)m_allocator->allocate( sizeof(memory_batch) );
	m_push_pointer	= nullptr;
	m_last_pointer	= m_data->data;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
array_heap<ElemSize, BatchSize, Allocator>::~array_heap( )
{
	destroy( );
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
void array_heap<ElemSize, BatchSize, Allocator>::destroy( )
{
	m_allocator->deallocate( m_data );
	m_data = nullptr;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
pointer array_heap<ElemSize, BatchSize, Allocator>::allocate( uptr size )
{
	ASSERT				( size == ElemSize );

	pointer result		= nullptr;

	if ( m_push_pointer == nullptr )
	{
		ASSERT			( m_last_pointer < m_data->data + ElemSize * BatchSize ); // heap is full
		result			= m_last_pointer;
		m_last_pointer	+= ElemSize;
	}
	else
	{
		result			= m_push_pointer;
		m_push_pointer	= *(pointer*)m_push_pointer;
	}
	return result;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
void array_heap<ElemSize, BatchSize, Allocator>::deallocate( pointer p )
{
	ASSERT				( p );
	*(pointer*)pointer	= m_push_pointer;
	m_push_pointer		= p;
}


template<uptr ElemSize, uptr BatchSize, typename Allocator>
array_allocation_heap<ElemSize, BatchSize, Allocator>::array_allocation_heap( Allocator* allocator )
{
	m_allocator		= allocator;
	m_data			= (memory_batch*)m_allocator->allocate( sizeof(memory_batch) );
	m_push_pointer	= m_data->data;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
array_allocation_heap<ElemSize, BatchSize, Allocator>::~array_allocation_heap( )
{
	destroy( );
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
void array_allocation_heap<ElemSize, BatchSize, Allocator>::destroy( )
{
	m_allocator->deallocate( m_data );
	m_data = nullptr;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
pointer array_allocation_heap<ElemSize, BatchSize, Allocator>::allocate( uptr size )
{
	ASSERT			( size == ElemSize );
	ASSERT			( m_push_pointer < m_data->data + ElemSize * BatchSize ); // heap is full
	pointer result	= m_push_pointer;
	m_push_pointer	+= ElemSize;
	return			result;
}


template<uptr ElemSize, uptr BatchSize, typename Allocator>
dynamic_array_heap<ElemSize, BatchSize, Allocator>::dynamic_array_heap( Allocator* allocator )
{
	ASSERT			( ElemSize >= sizeof(pointer) );
	m_allocator		= allocator;
	m_data			= nullptr;
	m_data->prev	= nullptr;
	m_push_pointer	= nullptr;
	m_last_pointer	= m_data->data + ElemSize * BatchSize;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
dynamic_array_heap<ElemSize, BatchSize, Allocator>::~dynamic_array_heap( )
{
	destroy( );
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
void dynamic_array_heap<ElemSize, BatchSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		m_allocator->deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
pointer dynamic_array_heap<ElemSize, BatchSize, Allocator>::allocate( uptr size )
{
	ASSERT( size == ElemSize );

	pointer result = nullptr;

	if ( m_push_pointer == nullptr )
	{
		if ( m_last_pointer != m_data->data + ElemSize * BatchSize )
		{
			pointer result		= m_last_pointer;
			m_last_pointer		+= ElemSize;
			return result;
		}
		else
		{
			memory_batch* mem	= m_data;
			m_data				= (memory_batch*)m_allocator->allocate( sizeof(memory_batch) );
			m_data->prev		= mem;
			m_last_pointer		= m_data->data + ElemSize;
			return m_data->data;
		}
	}
	else
	{
		pointer result			= m_push_pointer;
		m_push_pointer			= *(pointer*)m_push_pointer;
		return result;
	}
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
void dynamic_array_heap<ElemSize, BatchSize, Allocator>::deallocate( pointer p )
{
	ASSERT				( p );
	*(pointer*)p		= m_push_pointer;
	m_push_pointer		= p;
}


template<uptr ElemSize, uptr BatchSize, typename Allocator>
dynamic_array_allocation_heap<ElemSize, BatchSize, Allocator>::dynamic_array_allocation_heap( Allocator* allocator )
{
	m_allocator		= allocator;
	m_data			= nullptr;
	m_data->prev	= nullptr;
	m_push_pointer	= m_data->data + ElemSize * BatchSize;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
dynamic_array_allocation_heap<ElemSize, BatchSize, Allocator>::~dynamic_array_allocation_heap( )
{
	destroy( );
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
void dynamic_array_allocation_heap<ElemSize, BatchSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		m_allocator->deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<uptr ElemSize, uptr BatchSize, typename Allocator>
pointer dynamic_array_allocation_heap<ElemSize, BatchSize, Allocator>::allocate( uptr size )
{
	ASSERT( size == ElemSize );

	if ( m_push_pointer != m_data->data + ElemSize * BatchSize )
	{
		pointer result		= m_push_pointer;
		m_push_pointer		+= ElemSize;
		return result;
	}
	else
	{
		memory_batch* mem	= m_data;
		m_data				= (memory_batch*)m_allocator->allocate( sizeof(memory_batch) );
		m_data->prev		= mem;
		m_push_pointer		= m_data->data + ElemSize;
		return m_data->data; 
	}
}


template<uptr SizeMin, uptr SizeMax, uptr SizeStep, uptr PageSize, typename Allocator>
dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::dynamic_heap( Allocator* allocator )
{
	ASSERT			( SizeMin >= sizeof(pointer) );
	ASSERT			( SizeMax <= PageSize );
	ASSERT			( ( SizeMax - SizeMin ) % SizeStep != 0 );
	m_allocator		= allocator;
	m_data			= nullptr;
	m_data->prev	= nullptr;
	memset			( m_push_pointers, 0, sizeof(pointer) * push_pointers_count );
	m_last_pointer	= m_data->data + PageSize;
}

template<uptr SizeMin, uptr SizeMax, uptr SizeStep, uptr PageSize, typename Allocator>
dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::~dynamic_heap( )
{
	destroy( );
}

template<uptr SizeMin, uptr SizeMax, uptr SizeStep, uptr PageSize, typename Allocator>
void dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		m_allocator->deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<uptr SizeMin, uptr SizeMax, uptr SizeStep, uptr PageSize, typename Allocator>
pointer dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::allocate( uptr size )
{
	ASSERT						( size < SizeMin );
	ASSERT						( size > SizeMax );
	ASSERT						( ( size - SizeMin ) % SizeStep );
	u32 const index				= ( size - SizeMin ) / SizeStep;

	if ( m_push_pointers[index] == nullptr )
	{
		if ( m_last_pointer + size <= m_data->data + PageSize )
		{
			pointer result		= m_last_pointer;
			m_last_pointer		+= size;
			return result;
		}
		else
		{
			memory_batch* mem	= m_data;
			m_data				= (memory_batch*)m_allocator->allocate( sizeof(memory_batch) );
			m_data->prev		= mem;
			m_last_pointer		= m_data->data + size;
			return m_data->data;
		}
	}
	else
	{
		pointer result			= m_push_pointers[index];
		m_push_pointers[index]	= *(pointer*)m_push_pointers[index];
		return result;
	}
}

template<uptr SizeMin, uptr SizeMax, uptr SizeStep, uptr PageSize, typename Allocator>
void dynamic_heap<SizeMin, SizeMax, SizeStep, PageSize, Allocator>::deallocate( pointer p, uptr size )
{
	ASSERT					( p );

	ASSERT					( size < SizeMin );
	ASSERT					( size > SizeMin );
	ASSERT					( ( size - SizeMin ) % SizeStep );
	u32 const index			= ( size - SizeMin ) / SizeStep;

	*(pointer*)p			= m_push_pointers[index];
	m_push_pointer[index]	= p;
}


template<uptr PageSize, typename Allocator>
dynamic_allocation_heap<PageSize, Allocator>::dynamic_allocation_heap( Allocator* allocator )
{
	m_allocator		= allocator;
	m_data			= nullptr;
	m_data->prev	= nullptr;
	m_last_pointer	= m_data->data + PageSize;
}

template<uptr PageSize, typename Allocator>
dynamic_allocation_heap<PageSize, Allocator>::~dynamic_allocation_heap( )
{
	destroy( );
}

template<uptr PageSize, typename Allocator>
void dynamic_allocation_heap<PageSize, Allocator>::destroy( )
{
	memory_batch* mem = m_data;
	while ( mem )
	{
		memory_batch* prev = mem->prev;
		m_allocator->deallocate( mem );
		mem = prev;
	}
	m_data = nullptr;
}

template<uptr PageSize, typename Allocator>
pointer dynamic_allocation_heap<PageSize, Allocator>::allocate( uptr size )
{
	if ( m_last_pointer + size <= m_data->data + PageSize )
	{
		pointer result		= m_last_pointer;
		m_last_pointer		+= size;
		return result;
	}
	else
	{
		memory_batch* mem	= m_data;
		m_data				= (memory_batch*)m_allocator->allocate( sizeof(memory_batch) );
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

#ifndef __core_separate_arena_temporal_allocator_inline_h_included_
#define __core_separate_arena_temporal_allocator_inline_h_included_

#include <macros.h>

template<u32 PageSize, u32 PagesPerThread, u32 ThreadMaxCount>
void separate_arena_temporal_allocator<PageSize, PagesPerThread, ThreadMaxCount>::create( )
{
	m_data									= virtual_allocator( ).reserve( nullptr, page_size * page_count );
	
	for ( u32 i = 0; i < page_count; ++i )
		m_pages_occupancy[i]				= 0;

	for ( u32 i = 0; i < ThreadMaxCount; ++i )
	{
		m_active_page_data[i].occupancy		= 0;
		m_active_page_data[i].index			= PagesPerThread * i;
	}
}

template<u32 PageSize, u32 PagesPerThread, u32 ThreadMaxCount>
void separate_arena_temporal_allocator<PageSize, PagesPerThread, ThreadMaxCount>::destroy( )
{
#ifdef DEBUG
	for ( u32 i = 0; i < page_count; ++i )
		if ( i != m_active_page_data[i / PagesPerThread].index )
		{
			ASSERT_CMP						( m_pages_occupancy[i], ==, 0 );
		}
		else
		{
			ASSERT_CMP						( 0 - m_pages_occupancy[i], ==, m_active_page_data[i / PagesPerThread].occupancy );
		}
#endif // #ifdef DEBUG

	virtual_allocator( ).release			( m_data );
}

template<u32 PageSize, u32 PagesPerThread, u32 ThreadMaxCount>
pointer separate_arena_temporal_allocator<PageSize, PagesPerThread, ThreadMaxCount>::allocate( uptr const size, u32 const thread_id )
{
	ASSERT_CMP								( size, <=, page_size );
	ASSERT_CMP								( thread_id, <=, ThreadMaxCount );

	u32 const real_size						= (u32)size;

	u32 const page_index					= m_active_page_data[thread_id].index;
	u32 const page_occupancy				= m_active_page_data[thread_id].occupancy;
	u32 const new_page_occupancy			= page_occupancy + real_size;

	if ( new_page_occupancy <= page_size )
	{
		pointer const result				= m_data + page_size * page_index + page_occupancy;
		m_active_page_data[thread_id].occupancy = new_page_occupancy;
		return result;
	}

	u32 const page_real_occupancy			= interlocked_add( m_pages_occupancy[page_index], page_occupancy );

	if ( page_real_occupancy == 0 )
	{
		pointer const result				= m_data + page_size * page_index;
		m_active_page_data[thread_id].occupancy = real_size;
		return result;
	}

	u32 new_page_index						= page_index;

	do
	{
		new_page_index						= ( new_page_index % PagesPerThread != PagesPerThread - 1 ) ? new_page_index + 1 : new_page_index + 1 - PagesPerThread;
		ASSERT								( new_page_index != page_index, "failed to allocate" );
	}
	while ( m_pages_occupancy[new_page_index] != 0 );

	pointer const result					= m_data + page_size * new_page_index;
	virtual_allocator( ).commit				( result, page_size );

	m_active_page_data[thread_id].index		= new_page_index;
	m_active_page_data[thread_id].occupancy	= real_size;

	return result;
}

template<u32 PageSize, u32 PagesPerThread, u32 ThreadMaxCount>
void separate_arena_temporal_allocator<PageSize, PagesPerThread, ThreadMaxCount>::deallocate( pointer const p, uptr const size )
{
	ASSERT_CMP								( p, >=, m_data );
	ASSERT_CMP								( p, <, m_data + page_size * page_count );
	ASSERT_CMP								( size, <, page_size );

	u32 const inverse_size					= 0 - (u32)size;
	u32 const page_index					= (u32)( ( p - m_data ) / page_size );

	u32 const occupancy						= interlocked_add( m_pages_occupancy[page_index], inverse_size );
	if ( occupancy == 0 )
	{
		virtual_allocator( ).decommit		( m_data + page_size * page_index, page_size );
	}
}

template<u32 PageSize, u32 PagesPerThread, u32 ThreadMaxCount>
pointer separate_arena_temporal_allocator<PageSize, PagesPerThread, ThreadMaxCount>::data( ) const
{
	return m_data;
}

#endif // #ifndef __core_separate_arena_temporal_allocator_inline_h_included_
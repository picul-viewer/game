#ifndef __core_temporal_allocator_inline_h_included_
#define __core_temporal_allocator_inline_h_included_

#include <macros.h>
#include <lib/allocator.h>

template<u32 PageMaxCount>
void temporal_allocator<PageMaxCount>::create( )
{
	m_data									= virtual_allocator( ).reserve( nullptr, page_size * PageMaxCount );
	
	for ( u32 i = 0; i < PageMaxCount; ++i )
		m_pages_occupancy[i]				= 0;

	m_active_page_index_and_occupancy		= 0;
}

template<u32 PageMaxCount>
void temporal_allocator<PageMaxCount>::destroy( )
{
#ifdef DEBUG
	for ( u32 i = 0; i < PageMaxCount; ++i )
		if ( i != m_active_page_index )
		{
			ASSERT_CMP						( m_pages_occupancy[i], ==, 0 );
		}
		else
		{
			ASSERT_CMP						( 0 - m_pages_occupancy[i], ==, m_active_page_occupancy );
		}
#endif // #ifdef DEBUG

	virtual_allocator( ).release			( m_data );
}

template<u32 PageMaxCount>
pointer temporal_allocator<PageMaxCount>::allocate( uptr const size )
{
	ASSERT_CMP								( size, <=, page_size );
	u32 const real_size						= (u32)size;

	while ( true )
	{
		u64 const value						= interlocked_add( m_active_page_index_and_occupancy, real_size );
		u32 const new_offset				= value & 0xFFFFFFFF;
		u32 const current_offset			= new_offset - real_size;
		u32 const page_index				= value >> 32;

		if ( new_offset <= page_size )
		{
			pointer const result			= m_data + ( page_size * page_index + current_offset );
			return result;
		}

		if ( current_offset <= page_size )
		{
			u32 const page_real_occupancy	= interlocked_add( m_pages_occupancy[page_index], current_offset );

			if ( page_real_occupancy == 0 )
			{
				// Reuse current page.
				m_active_page_occupancy		= 0;
			}
			else
			{
				u32 new_page_index			= page_index;

				do
				{
					new_page_index			= ( new_page_index + 1 ) % PageMaxCount;
					ASSERT					( new_page_index != page_index, "failed to allocate" );
				}
				while ( m_pages_occupancy[new_page_index] != 0 );

				virtual_allocator( ).commit	( m_data + page_size * new_page_index, page_size );

				u64 const new_page_data		= (u64)new_page_index << 32;
				m_active_page_index_and_occupancy = new_page_data;
			}
		}
		else
		{
			SPIN_LOCK						( m_active_page_occupancy_high != 0 );
		}
	}
}

template<u32 PageMaxCount>
void temporal_allocator<PageMaxCount>::deallocate( pointer const p, uptr const size )
{
	ASSERT_CMP								( p, >=, m_data );
	ASSERT_CMP								( p, <, m_data + page_size * PageMaxCount );
	ASSERT_CMP								( size, <, page_size );

	u32 const inverse_size					= 0 - (u32)size;
	u32 const page_index					= (u32)( ( p - m_data ) / page_size );

	u32 const occupancy						= interlocked_add( m_pages_occupancy[page_index], inverse_size );
	if ( occupancy == 0 )
	{
		virtual_allocator( ).decommit		( m_data + page_size * page_index, page_size );
	}
}

template<u32 PageMaxCount>
pointer temporal_allocator<PageMaxCount>::data( ) const
{
	return m_data;
}

#endif // #ifndef __core_temporal_allocator_inline_h_included_
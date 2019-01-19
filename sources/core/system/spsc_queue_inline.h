#ifndef __core_spsc_queue_inline_h_inlcuded_
#define __core_spsc_queue_inline_h_inlcuded_

#include <lib/allocator.h>

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::create( pointer const data, uptr const size )
{
	static_assert				( sizeof(T) <= RecordSize, "incorrect template parameters" );

	ASSERT_CMP					( size % RecordSize, ==, 0 );
	ASSERT_CMP					( size / RecordSize, <=, 0xFFFFFFFF );

	u32 const array_size		= (u32)( size / RecordSize );
	ASSERT						( ( array_size & ( array_size - 1 ) ) == 0 );

	m_data						= data;
	m_index_mask				= array_size - 1;

	m_push_index				= 0;
	m_pop_index					= 0;

	m_current_size				= 0;

	m_empty_event.create		( false, false );
}

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::destroy( )
{
	m_empty_event.destroy		( );
}

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::push( value_type const& value )
{
	u32 const push_index		= m_push_index;
	u32 const new_push_index	= push_index + 1;
	
	m_data[push_index & m_index_mask] = value;
	
	m_push_index				= new_push_index;
	
	store_fence					( );

	if ( interlocked_inc( m_current_size ) == 0 )
	{
		m_empty_event.set		( );
	}
}

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::push( value_type const* const values, u32 const count )
{
	u32 const push_index		= m_push_index;
	u32 const new_push_index	= push_index + count;

	for ( u32 i = push_index, j = 0; i < new_push_index; ++i, ++j )
		m_data[i & m_index_mask] = values[j];

	m_push_index				= new_push_index;

	store_fence					( );

	if ( interlocked_add( m_current_size, count ) == count - 1 )
	{
		m_empty_event.set		( );
	}
}

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::pop( value_type& value )
{
	if ( interlocked_exchange_add( m_current_size, (u32)-1 ) == 0 )
	{
		m_empty_event.wait_for	( );
	}
	
	u32 const pop_index			= m_pop_index;
	u32 const new_pop_index		= pop_index + 1;
	
	value						= m_data[pop_index & m_index_mask];
	
	m_pop_index					= new_pop_index;
}

template<typename T, uptr RecordSize>
pointer spsc_queue<T, RecordSize>::data( ) const
{
	return m_data;
}

#endif // #ifndef __core_spsc_queue_inline_h_inlcuded_

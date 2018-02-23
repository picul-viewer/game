#ifndef __core_spsc_queue_inline_h_inlcuded_
#define __core_spsc_queue_inline_h_inlcuded_

#include <lib/allocator.h>

template<typename T, uptr RecordSize>
void spsc_queue_no_wait<T, RecordSize>::create( uptr in_buffer_size )
{
	ASSERT					( sizeof(T) <= RecordSize );
	ASSERT					( in_buffer_size % RecordSize == 0 );

	u32 const array_size	= (u32)( in_buffer_size / RecordSize );

	ASSERT					( ( array_size & ( array_size - 1 ) ) == 0 );

	m_data					= aligned_mem_allocator<Cache_Line>( ).allocate( in_buffer_size );
	m_index_mask			= array_size - 1;

	m_push_index			= 0;
	m_pop_index				= 0;
}

template<typename T, uptr RecordSize>
void spsc_queue_no_wait<T, RecordSize>::destroy( )
{
	aligned_mem_allocator<Cache_Line>( ).deallocate( m_data );
}

template<typename T, uptr RecordSize>
void spsc_queue_no_wait<T, RecordSize>::push( value_type const& in_value )
{
	value_type* v		= m_data + ( m_push_index & m_index_mask );
	*v					= in_value;
	
	s32 new_push_index	= interlocked_inc( m_push_index );
}

template<typename T, uptr RecordSize>
void spsc_queue_no_wait<T, RecordSize>::pop( value_type& out_value )
{
	out_value			= *( m_data + ( m_pop_index & m_index_mask ) );
	
	s32 new_pop_index	= interlocked_inc( m_pop_index );
}

template<typename T, uptr RecordSize>
bool spsc_queue_no_wait<T, RecordSize>::empty( ) const
{
	return m_push_index - m_pop_index <= 0;
}

template<typename T, uptr RecordSize>
bool spsc_queue_no_wait<T, RecordSize>::full( ) const
{
	return m_push_index - m_pop_index >= m_index_mask + 1;
}


template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::create( uptr in_buffer_size )
{
	parent::create		( in_buffer_size );

	m_empty_event.create( false, false );
	m_full_event.create	( false, false );
}

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::destroy( )
{
	parent::destroy			( );

	m_empty_event.destroy	( );
	m_full_event.destroy	( );
}

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::push( value_type const& in_value )
{
	// If there is no place to push
	while ( m_push_index - m_pop_index > m_index_mask )
	{
		m_full_event.wait_for		( );
	}

	value_type* v		= m_data + ( m_push_index & m_index_mask );
	*v					= in_value;
	
	s32 new_push_index	= interlocked_inc( m_push_index );

	if ( new_push_index - m_pop_index <= 1 )
	{
		m_empty_event.set			( );
	}
}

template<typename T, uptr RecordSize>
void spsc_queue<T, RecordSize>::pop( value_type& out_value )
{
	// If there is nothing to pop
	while ( m_push_index - m_pop_index <= 0 )
	{
		m_empty_event.wait_for	( );
	}
	
	out_value			= *( m_data + ( m_pop_index & m_index_mask ) );
	
	s32 new_pop_index	= interlocked_inc( m_pop_index );

	if ( m_push_index - new_pop_index >= m_index_mask )
	{
		m_full_event.set		( );
	}
}

#endif // #ifndef __core_spsc_queue_inline_h_inlcuded_

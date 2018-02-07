#ifndef __core_task_queue_inline_h_inlcuded_
#define __core_task_queue_inline_h_inlcuded_

template<uptr RecordSize>
void task_queue<RecordSize>::create( uptr in_buffer_size )
{
	ASSERT					( in_buffer_size % RecordSize == 0 );

	u32 const array_size	= (u32)( in_buffer_size / RecordSize );

	ASSERT					( ( array_size & ( array_size - 1 ) ) == 0 );

	m_data					= aligned_mem_allocator<Cache_Line>( ).allocate( in_buffer_size );
	m_index_mask			= array_size - 1;

	m_push_index			= 0;
	m_pop_index				= 0;

	m_empty_event.create	( false, false );
	m_full_event.create		( false, false );
}

template<uptr RecordSize>
void task_queue<RecordSize>::destroy( )
{
	aligned_mem_allocator<Cache_Line>( ).deallocate( m_data );
}

template<uptr RecordSize>
template<typename ... Args>
void task_queue<RecordSize>::push( void(*in_functor)( Args ... ), Args ... args )
{
	// If there is no place to push
	while ( m_push_index - m_pop_index > m_index_mask )
	{
		m_full_event.wait_for		( );
	}

	functor* f						= m_data + ( m_push_index & m_index_mask );
	new ( f ) functor				( in_functor, args ... );
	
	s32 new_push_index				= interlocked_inc( m_push_index );

	if ( new_push_index - m_pop_index <= 1 )
	{
		m_empty_event.set			( );
	}
}

template<uptr RecordSize>
template<typename T, typename ... Args>
void task_queue<RecordSize>::push( void(T::*in_functor)( Args ... ), T* this_ptr, Args ... args )
{
	push( (void(*)( T*, Args... ))in_functor, this_ptr, args );
}

template<uptr RecordSize>
template<typename T, typename ... Args>
void task_queue<RecordSize>::push( void(T::*in_functor)( Args ... )const, T const* this_ptr, Args ... args )
{
	push( (void(*)( T const*, Args... ))in_functor, this_ptr, args );
}

template<uptr RecordSize>
void task_queue<RecordSize>::pop( functor& out_functor )
{
	// If there is nothing to pop
	while ( m_push_index - m_pop_index <= 0 )
	{
		m_empty_event.wait_for	( );
	}
	
	out_functor						= *( m_data + ( m_pop_index & m_index_mask ) );
	
	s32 new_pop_index				= interlocked_inc( m_pop_index );

	if ( m_push_index - new_pop_index >= m_index_mask )
	{
		m_full_event.set			( );
	}
}

template<uptr RecordSize>
bool task_queue<RecordSize>::empty( ) const
{
	return m_push_index - m_pop_index <= 0;
}

template<uptr RecordSize>
bool task_queue<RecordSize>::full( ) const
{
	return m_push_index - m_pop_index >= m_index_mask + 1;
}

#endif // #ifndef __core_task_queue_inline_h_inlcuded_

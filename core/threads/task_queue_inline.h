#ifndef __core_task_queue_inline_h_inlcuded_
#define __core_task_queue_inline_h_inlcuded_

template<typename ... Args>
void task_queue::push( void(*in_functor)( Args ... ), Args ... args )
{
	ASSERT				( sizeof ... (Args) <= c_max_task_parameters_size );

	s8* pointer			= m_push_pointer;

	*(functor*)pointer	= (functor)in_functor;
	pointer				+= sizeof(functor);

	*(u32*)pointer		= sizeof ... (Args);
	pointer				+= sizeof(u32);

	// Get pointer on args
	s8* args_pointer	= (s8*)in_functor + sizeof( void(*)( Args ... ) );

	memcpy( pointer, args_pointer, sizeof ... (Args) );
	pointer				+= sizeof ... (Args);

	m_push_pointer		= ( pointer <= m_data_end ) ? pointer : m_data.data( );
}

template<typename T, typename ... Args>
void task_queue::push( void(T::*in_functor)( Args ... ), T* this_ptr, Args ... args )
{
	push( (void(*)( T*, Args... ))in_functor, this_ptr, args );
}

template<typename T, typename ... Args>
void task_queue::push( void(T::*in_functor)( Args ... )const, T const* this_ptr, Args ... args )
{
	push( (void(*)( T const*, Args... ))in_functor, this_ptr, args );
}


template<typename ... Args>
void thread_task_queue::push( void(*in_functor)( Args ... ), Args ... args )
{
	ASSERT				( sizeof ... (Args) <= c_max_task_parameters_size );

	s8* pointer			= m_push_pointer;

	*(functor*)pointer	= (functor)in_functor;
	pointer				+= sizeof(functor);

	*(u32*)pointer		= sizeof ... (Args);
	pointer				+= sizeof(u32);

	// Get pointer on args
	s8* args_pointer	= (s8*)in_functor + sizeof( void(*)( Args ... ) );

	memcpy( pointer, args_pointer, sizeof ... (Args) );
	pointer				+= sizeof ... (Args);

	m_push_pointer		= ( pointer <= m_data_end ) ? pointer : m_data.data( );

	if ( m_thread_sleeping )
	{
		m_thread_sleeping = false;
		m_worker_thread->resume( );
	}
}

template<typename T, typename ... Args>
void thread_task_queue::push( void(T::*in_functor)( Args ... ), T* this_ptr, Args ... args )
{
	push( (void(*)( T*, Args... ))in_functor, this_ptr, args );
}

template<typename T, typename ... Args>
void thread_task_queue::push( void(T::*in_functor)( Args ... )const, T const* this_ptr, Args ... args )
{
	push( (void(*)( T const*, Args... ))in_functor, this_ptr, args );
}

#endif // #ifndef __core_task_queue_inline_h_inlcuded_

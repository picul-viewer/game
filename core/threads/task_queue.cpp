#include "task_queue.h"

void task_queue::create( )
{
	m_data_end		= m_data.data( ) + c_task_queue_size - c_max_task_parameters_size - sizeof(functor) - sizeof(u32);

	m_push_pointer	= m_data.data( );
	m_pop_pointer	= m_data.data( );
}

void task_queue::destroy( )
{
	//
}

void task_queue::pop( functor& out_functor, va_list& out_args_list )
{
	// If there is nothing to push
	if ( empty( ) )
	{
		out_functor = empty_func;
		return;
	}

	s8* pointer			= m_pop_pointer;

	out_functor			= *(functor*)pointer;
	pointer				+= sizeof(functor);

	u32 parameters_size	= *(u32*)pointer;
	pointer				+= sizeof(u32);

	out_args_list		= pointer;
	pointer				+= parameters_size;

	m_pop_pointer		= ( pointer <= m_data_end ) ? pointer : m_data.data( );
}

bool task_queue::empty( ) const
{
	return m_push_pointer == m_pop_pointer;
}

void task_queue::empty_func( va_list args )
{ }


void thread_task_queue::create( thread* in_thread )
{
	m_worker_thread		= in_thread;
	m_thread_sleeping	= false;
	m_data_end			= m_data.data( ) + c_task_queue_size - c_max_task_parameters_size - sizeof(functor) - sizeof(u32);

	m_push_pointer		= m_data.data( );
	m_pop_pointer		= m_data.data( );
}

void thread_task_queue::destroy( )
{
	//
}

void thread_task_queue::pop( functor& out_functor, va_list& out_args_list )
{
	// If there is nothing to push
	if ( empty( ) )
	{
		m_worker_thread->suspend( );
		m_thread_sleeping	= true;
		return;
	}

	s8* pointer			= m_pop_pointer;

	out_functor			= *(functor*)pointer;
	pointer				+= sizeof(functor);

	u32 parameters_size	= *(u32*)pointer;
	pointer				+= sizeof(u32);

	out_args_list		= pointer;
	pointer				+= parameters_size;

	m_pop_pointer		= ( pointer <= m_data_end ) ? pointer : m_data.data( );
}

bool thread_task_queue::empty( ) const
{
	return m_push_pointer == m_pop_pointer;
}
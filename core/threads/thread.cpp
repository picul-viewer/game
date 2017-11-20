#include "thread.h"

#include <process.h>

thread::thread( thread_func_type func, u32 stack_size, void* arg )
{
	create( func, stack_size, arg );
}

void thread::create( thread_func_type func, u32 stack_size, void* arg )
{
	m_id = (HANDLE)_beginthread( func, stack_size, arg );
}

void thread::destroy( u32 wait_ms )
{
	WaitForSingleObject( m_id, wait_ms );
}

void thread::suspend( )
{
	SuspendThread( m_id );
}

void thread::resume( )
{
	ResumeThread( m_id );
}
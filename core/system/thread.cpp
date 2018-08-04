#include "thread.h"

#include <process.h>
#include <Windows.h>

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

void thread::destroy( u32 wait_ms, u32 count )
{
	WaitForMultipleObjects( count, &m_id, TRUE, wait_ms );
}

void thread::destroy_any( u32 wait_ms, u32 count )
{
	WaitForMultipleObjects( count, &m_id, FALSE, wait_ms );
}

void thread::suspend( )
{
	SuspendThread( m_id );
}

void thread::resume( )
{
	ResumeThread( m_id );
}
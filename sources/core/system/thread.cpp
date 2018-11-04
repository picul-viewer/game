#include "thread.h"
#include <macros.h>
#include <process.h>
#include <Windows.h>

thread::thread( thread_func_type func, u32 stack_size, pvoid arg )
{
	create( func, stack_size, arg );
}

void thread::create( thread_func_type func, u32 stack_size, pvoid arg )
{
	m_id = (HANDLE)CreateThread( nullptr, stack_size, func, arg, 0, nullptr );
}

void thread::destroy( u32 wait_ms )
{
	WaitForSingleObject( m_id, wait_ms );
}

void thread::destroy( u32 wait_ms, u32 count )
{
	ASSERT( count != 0 );
	WaitForMultipleObjects( count, &m_id, TRUE, wait_ms );
}

void thread::destroy_any( u32 wait_ms, u32 count )
{
	ASSERT( count != 0 );
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
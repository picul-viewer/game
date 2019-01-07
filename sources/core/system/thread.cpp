#include "thread.h"
#include <macros.h>
#include <math/math_common.h>
#include <process.h>
#include <Windows.h>

thread::thread( thread_func_type const func, u32 const stack_size, pvoid const arg )
{
	create( func, stack_size, arg );
}

void thread::create( thread_func_type const func, u32 const stack_size, pvoid const arg )
{
	m_id = (HANDLE)CreateThread( nullptr, stack_size, func, arg, 0, nullptr );
	ASSERT( m_id != nullptr );
}

void thread::destroy( u32 const wait_ms )
{
	WaitForSingleObject( m_id, wait_ms );
}

void thread::destroy( u32 const wait_ms, u32 const count )
{
	ASSERT( count != 0 );
	WaitForMultipleObjects( count, &m_id, TRUE, wait_ms );
}

void thread::destroy_any( u32 const wait_ms, u32 const count )
{
	ASSERT( count != 0 );
	WaitForMultipleObjects( count, &m_id, FALSE, wait_ms );
}

void thread::suspend( )
{
	DWORD const result = SuspendThread( m_id );
	ASSERT( result != (DWORD)-1 );
}

void thread::resume( )
{
	DWORD const result = ResumeThread( m_id );
	ASSERT( result != (DWORD)-1 );
}

void thread::force_processor_index( u32 const index )
{
	set_affinity_mask( 1ull << index );
}

void thread::advise_processor_index( u32 const index )
{
	DWORD const result = SetThreadIdealProcessor( m_id, index );
	ASSERT( result != (DWORD)-1 );
}

void thread::set_priority( u32 const priority )
{
	BOOL const result = SetThreadPriority( m_id, math::max( priority, max_priority ) + THREAD_PRIORITY_IDLE );
	ASSERT( result != 0 );
}

void thread::set_affinity_mask( u64 const mask )
{
	DWORD_PTR const result = SetThreadAffinityMask( m_id, mask );
	ASSERT( result != 0 );
}
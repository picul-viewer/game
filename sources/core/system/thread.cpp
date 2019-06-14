#include "thread.h"
#include <macros.h>
#include <math/math_common.h>
#include <process.h>
#include <Windows.h>

void sys::thread::create( thread_func_type const func, u32 const stack_size, pvoid const arg )
{
	m_id = CreateThread( nullptr, stack_size, func, arg, 0, nullptr );
	ASSERT( m_id != nullptr );
}

void sys::thread::destroy( u32 const wait_ms )
{
	WaitForSingleObject( m_id, wait_ms );
}

void sys::thread::destroy( u32 const count, thread* const threads, u32 const wait_ms )
{
	ASSERT( count != 0 );

	WaitForMultipleObjects( count, &threads[0].m_id, TRUE, wait_ms );

	for ( u32 i = 0; i < count; ++i )
		CloseHandle( threads[i].m_id );
}

sys::thread sys::thread::get_current( )
{
	thread result;
	result.m_id = GetCurrentThread( );
	return result;
}

void sys::thread::suspend( )
{
	DWORD const result = SuspendThread( m_id );
	ASSERT( result != (DWORD)-1 );
}

void sys::thread::resume( )
{
	DWORD const result = ResumeThread( m_id );
	ASSERT( result != (DWORD)-1 );
}

void sys::thread::force_processor_index( u32 const index )
{
	set_affinity_mask( 1ull << index );
}

void sys::thread::advise_processor_index( u32 const index )
{
	DWORD const result = SetThreadIdealProcessor( m_id, index );
	ASSERT( result != (DWORD)-1 );
}

void sys::thread::set_priority( u32 const priority )
{
	BOOL const result = SetThreadPriority( m_id, math::min( priority, max_priority ) + THREAD_PRIORITY_IDLE );
	ASSERT( result != 0 );
}

void sys::thread::set_affinity_mask( u64 const mask )
{
	DWORD_PTR const result = SetThreadAffinityMask( m_id, mask );
	ASSERT( result != 0 );
}

u32 sys::thread::hardware_concurrency( )
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	return (u32)sysinfo.dwNumberOfProcessors;
}
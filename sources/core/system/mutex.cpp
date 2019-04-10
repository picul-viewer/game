#include "mutex.h"
#include <macros.h>
#include <Windows.h>

void sys::mutex::create( bool const own_mutex )
{
	m_id = CreateMutex( nullptr, own_mutex ? TRUE : FALSE, "" );
}

void sys::mutex::destroy( )
{
	CloseHandle( m_id );
}

void sys::mutex::wait( u32 const wait_ms )
{
	WaitForSingleObject( m_id, wait_ms );
}

void sys::mutex::wait_all( u32 const count, mutex* const objects, u32 const wait_ms )
{
	ASSERT( count != 0 );

	DWORD const result = WaitForMultipleObjects( count, &objects[0].m_id, TRUE, wait_ms );

	ASSERT( ( result < WAIT_ABANDONED_0 ) || ( result >= WAIT_ABANDONED_0 + count ) );
}

u32 sys::mutex::wait_any( u32 const count, mutex* const objects, u32 const wait_ms )
{
	ASSERT( count != 0 );

	DWORD const result = WaitForMultipleObjects( count, &objects[0].m_id, FALSE, wait_ms );

	ASSERT( ( result < WAIT_ABANDONED_0 ) || ( result >= WAIT_ABANDONED_0 + count ) );

	if ( result == WAIT_TIMEOUT )
		return 0xFFFFFFFF;

	return result - WAIT_OBJECT_0;
}

void sys::mutex::release( )
{
	BOOL const result = ReleaseMutex( m_id );
}


sys::mutex_lock::mutex_lock( mutex& m, u32 const wait_ms ) :
	m_mutex( m )
{
	m_mutex.wait( wait_ms );
}

sys::mutex_lock::~mutex_lock( )
{
	m_mutex.release( );
}
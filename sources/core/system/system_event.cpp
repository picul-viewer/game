#include "system_event.h"
#include <macros.h>
#include <Windows.h>

void sys::system_event::create( bool const initial_state, bool const use_manual_reset )
{
	m_id = CreateEvent( nullptr, use_manual_reset ? TRUE : FALSE, initial_state ? TRUE : FALSE, "" );
}

void sys::system_event::destroy( )
{
	CloseHandle( m_id );
}

void sys::system_event::set( ) const
{
	SetEvent( m_id );
}

void sys::system_event::reset( ) const
{
	ResetEvent( m_id );
}

void sys::system_event::wait( u32 const wait_ms ) const
{
	WaitForSingleObject( m_id, wait_ms );
}

void sys::system_event::wait_all( u32 const count, system_event const* const objects, u32 const wait_ms )
{
	ASSERT( count != 0 );

	DWORD const result = WaitForMultipleObjects( count, &objects[0].m_id, TRUE, wait_ms );

	ASSERT( ( result < WAIT_ABANDONED_0 ) || ( result >= WAIT_ABANDONED_0 + count ) );
}

u32 sys::system_event::wait_any( u32 const count, system_event const* const objects, u32 const wait_ms )
{
	ASSERT( count != 0 );

	DWORD const result = WaitForMultipleObjects( count, &objects[0].m_id, FALSE, wait_ms );

	ASSERT( ( result < WAIT_ABANDONED_0 ) || ( result >= WAIT_ABANDONED_0 + count ) );

	if ( result == WAIT_TIMEOUT )
		return 0xFFFFFFFF;

	return result - WAIT_OBJECT_0;
}
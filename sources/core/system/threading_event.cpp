#include "threading_event.h"
#include <macros.h>
#include <process.h>
#include <Windows.h>

threading_event::threading_event( bool const initial_state, bool const use_manual_reset )
{
	create( initial_state, use_manual_reset );
}

void threading_event::create( bool const initial_state, bool const use_manual_reset )
{
	m_id = CreateEvent( nullptr, use_manual_reset ? TRUE : FALSE, initial_state ? TRUE : FALSE, "" );
}

void threading_event::destroy( )
{
	CloseHandle( m_id );
}

void threading_event::destroy( u32 const count )
{
	ASSERT( count != 0 );
	for ( threading_event *i = this, *e = this + count; i != e; ++i )
		i->destroy( );
}

void threading_event::set( )
{
	SetEvent( m_id );
}

void threading_event::reset( )
{
	ResetEvent( m_id );
}

void threading_event::wait_for( u32 const wait_ms )
{
	WaitForSingleObject( m_id, wait_ms );
}

void threading_event::wait_for( u32 const wait_ms, u32 const count )
{
	ASSERT( count != 0 );
	WaitForMultipleObjects( count, &m_id, TRUE, wait_ms );
}

u32 threading_event::wait_for_any( u32 const wait_ms, u32 const count )
{
	ASSERT( count != 0 );
	DWORD const result = WaitForMultipleObjects( count, &m_id, FALSE, wait_ms );
	return result - WAIT_OBJECT_0;
}
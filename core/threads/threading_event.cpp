#include "threading_event.h"

#include <process.h>

threading_event::threading_event( bool initial_state, bool use_manual_reset )
{
	create( initial_state, use_manual_reset );
}

void threading_event::create( bool initial_state, bool use_manual_reset )
{
	m_id = CreateEvent( nullptr, use_manual_reset ? TRUE : FALSE, initial_state ? TRUE : FALSE, "" );
}

void threading_event::destroy( )
{
	CloseHandle( m_id );
}

void threading_event::set( )
{
	SetEvent( m_id );
}

void threading_event::reset( )
{
	ResetEvent( m_id );
}

void threading_event::wait_for( u32 wait_ms )
{
	WaitForSingleObject( m_id, wait_ms );
}
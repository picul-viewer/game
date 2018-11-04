#ifndef __core_threading_event_h_included_
#define __core_threading_event_h_included_

#include <types.h>

class threading_event
{
public:
	threading_event( ) = default;
	~threading_event( ) = default;

	threading_event( bool initial_state, bool use_manual_reset = true );

	void create( bool initial_state = true, bool use_manual_reset = true );
	void destroy( );
	void destroy( u32 count );

	void set( );
	void reset( );

	void wait_for( u32 wait_ms = (u32)-1 );
	void wait_for( u32 wait_ms, u32 count );
	void wait_for_any( u32 wait_ms, u32 count );

protected:
	pvoid	m_id;
};

#endif // #ifndef __core_threading_event_h_included_
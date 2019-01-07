#ifndef __core_threading_event_h_included_
#define __core_threading_event_h_included_

#include <types.h>

class threading_event
{
public:
	threading_event( ) = default;
	~threading_event( ) = default;

	threading_event( bool const initial_state, bool const use_manual_reset = true );

	void create( bool const initial_state = true, bool const use_manual_reset = true );
	void destroy( );
	void destroy( u32 const count );

	void set( );
	void reset( );

	void wait_for( u32 const wait_ms = time::infinite );
	void wait_for( u32 const wait_ms, u32 const count );
	u32 wait_for_any( u32 const wait_ms, u32 const count );

protected:
	pvoid	m_id;
};

#endif // #ifndef __core_threading_event_h_included_
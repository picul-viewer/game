#ifndef GUARD_CORE_SYSTEM_EVENT_H_INCLUDED
#define GUARD_CORE_SYSTEM_EVENT_H_INCLUDED

#include <types.h>

namespace sys {

class system_event
{
public:
	void create( bool const initial_state = true, bool const use_manual_reset = true );
	void destroy( );

	void set( );
	void reset( );

	void wait( u32 const wait_ms = time::infinite );

	static void wait_all( u32 const count, system_event* const objects, u32 const wait_ms = time::infinite );
	static u32 wait_any( u32 const count, system_event* const objects, u32 const wait_ms = time::infinite );

	inline pvoid get_handle( ) const { return m_id; }

private:
	pvoid m_id;

};

} // namespace sys

#endif // #ifndef GUARD_CORE_SYSTEM_EVENT_H_INCLUDED
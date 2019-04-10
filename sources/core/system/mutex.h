#ifndef GUARD_CORE_MUTEX_H_INCLUDED
#define GUARD_CORE_MUTEX_H_INCLUDED

#include <types.h>

namespace sys {

class mutex
{
public:
	void create( bool const own_mutex = false );
	void destroy( );

	void wait( u32 const wait_ms = time::infinite );

	static void wait_all( u32 const count, mutex* const objects, u32 const wait_ms = time::infinite );
	static u32 wait_any( u32 const count, mutex* const objects, u32 const wait_ms = time::infinite );

	void release( );

	inline pvoid get_handle( ) const { return m_id; }

private:
	pvoid m_id;

};

class mutex_lock
{
public:
	mutex_lock( mutex& m, u32 const wait_ms = time::infinite );
	~mutex_lock( );

private:
	mutex& m_mutex;

};

} // namespace sys

#endif // #ifndef GUARD_CORE_MUTEX_H_INCLUDED
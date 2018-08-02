#ifndef __core_thread_h_included_
#define __core_thread_h_included_

#include <types.h>

class thread
{
public:
	thread( ) = default;
	~thread( ) = default;

	typedef void( *thread_func_type )( void* );

	thread( thread_func_type func, u32 stack_size, void* arg );
	
	void create( thread_func_type func, u32 stack_size, void* arg );
	void destroy( u32 wait_ms = 0 );
	void destroy( u32 wait_ms, u32 count );
	void destroy_any( u32 wait_ms, u32 count );

	void suspend( );
	void resume( );

protected:
	pvoid	m_id;
};

#endif // #ifndef __core_thread_h_included_

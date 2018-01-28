#include "fs_core.h"

#include <Shlwapi.h>

namespace fs {

namespace core {

thread		m_fs_thread;
fs_queue	m_fs_queue;
bool		m_fs_keep_alive;

void fs_thread_func( void* arg )
{
	bool *keep_alive = (bool*)arg;
	
	fs_queue::functor functor;

	while ( *keep_alive )
	{
		get_fs_queue( ).pop( functor );
		functor( );
	}

	while ( !get_fs_queue( ).empty( ) )
	{
		get_fs_queue( ).pop( functor );
		functor( );
	}
}

void create( )
{
	m_fs_keep_alive = true;

	m_fs_queue.create( 1 * Mb );
	m_fs_thread.create( fs_thread_func, 0, &m_fs_keep_alive );
}

void destroy( )
{
	m_fs_keep_alive = false;
}

void wait_for_destruction( )
{
	m_fs_thread.destroy( INFINITE );
}

fs_queue& get_fs_queue( )
{
	return m_fs_queue;
}

} // namespace core

} // namespace fs
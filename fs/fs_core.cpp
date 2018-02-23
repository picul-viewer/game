#include "fs_core.h"

#include <Shlwapi.h>
#include <threads/thread.h>

namespace fs {

namespace fs_core {

struct fs_thread_args
{
	bool keep_alive;
	bool thread_finished;
};

thread			m_fs_thread;
fs_queue		m_fs_queue;
fs_thread_args	m_fs_thread_args;

void fs_thread_func( void* arg )
{
	fs_thread_args *args = (fs_thread_args*)arg;
	
	fs_queue::value_type functor;

	while ( args->keep_alive )
	{
		get_fs_queue( ).pop( functor );
		functor( );
	}

	while ( !get_fs_queue( ).empty( ) )
	{
		get_fs_queue( ).pop( functor );
		functor( );
	}

	args->thread_finished = true;
}

void create( )
{
	m_fs_thread_args.keep_alive			= true;
	m_fs_thread_args.thread_finished	= false;

	m_fs_queue.create( 1 * Mb );
	m_fs_thread.create( fs_thread_func, 0, &m_fs_thread_args );
}

void destroy( )
{
	m_fs_thread_args.keep_alive		= false;
}

void wait_for_destruction( )
{
	while ( m_fs_thread_args.thread_finished == false )
		Sleep( 100 );
}

fs_queue& get_fs_queue( )
{
	return m_fs_queue;
}

} // namespace fs_core

bool file_exists( pcstr file_name )
{
	return PathFileExists( file_name ) == TRUE;
}

} // namespace fs
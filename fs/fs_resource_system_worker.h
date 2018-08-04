#ifndef __fs_fs_resource_system_worker_h_included_
#define __fs_fs_resource_system_worker_h_included_

#include <types.h>

#include <lib/function.h>

#include <system/thread.h>
#include <system/spsc_queue.h>

namespace fs {

class fs_resource_system_worker
{
public:
	fs_resource_system_worker( ) = default;

	void create( );
	void destroy( );
	void wait_for_destruction( );
	
	typedef fixed_function<void(*)( pointer, pointer, uptr ),
		pointer, bound_param_0, bound_param_1> callback_type;

	void read( pcstr filename, callback_type const& callback );

protected:
	struct query
	{
		pcstr filename;
		callback_type callback;
	};

	static void thread_func( void* arg );
	void work( );

	static void read_impl( query query );

	typedef spsc_queue<query> query_queue;

	thread		m_thread;
	query_queue	m_queue;
	bool		m_keep_alive;
};

} // namespace fs

#endif // #ifndef __fs_fs_resource_system_worker_h_included_
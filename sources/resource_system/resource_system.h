#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED

#include <types.h>
#include <lib/buffer_array.h>
#include <lib/function.h>
#include <lib/temporal_allocator.h>
#include <system/thread.h>
#include <system/mpsc_queue.h>
#include <system/mpmc_queue.h>

#include "resource_system_types.h"
#include "request_ptr.h"

namespace resource_system {

struct query_data;

class resource_system
{
public:
	void create( u32 const in_worker_thread_count );
	void destroy( );
	
	void process_request( request_ptr& in_request, query_callback_id const in_callback_id );
	void process_request_from_file( pcstr const in_file_path, query_callback_id const in_callback_id );
	void query_file( pcstr const in_file_path, query_callback_id const in_callback_id );
	void query_request( pcstr const in_file_path, query_callback_id const in_callback_id );

private:
	enum { max_worker_thread_count = 16 };

private:
	bool manage_resource( query_data* const in_data );

	void fs_thread( );
	void manager_thread( );
	void worker_thread( );

	void push_query( query_data* const in_query );

private:
	friend class request_ptr;
	void manage_query( query_data* const in_query );

private:
	mpsc_queue<query_data*> m_fs_queue;
	mpsc_queue<query_data*> m_manager_queue;
	mpmc_queue<query_data*> m_worker_queue;

	thread m_fs_thread;
	thread m_manager_thread;
	thread m_worker_threads[max_worker_thread_count];

	u32 m_worker_count;
};

extern resource_system g_resource_system;

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED

#include <types.h>
#include <lib/temporal_allocator.h>
#include <system/conditional_variable.h>
#include <system/critical_section.h>
#include <system/mpsc_queue.h>
#include <system/mpmc_queue.h>
#include <system/time.h>
#include <utils/engine_threads.h>

#include "resource_system_types.h"

namespace resource_system {

class resource_cook;

typedef pvoid query_result;

class resource_system
{
public:
	void create( u32 const in_thread_count );

	void stop( );
	void destroy( );
	
	void create_resources(
		query_info const* const in_queries,
		u32 const in_query_count,
		user_callback const& in_callback,
		pointer const in_callback_data,
		uptr const in_callback_data_size
	);

	void destroy_resource(
		query_info const& in_query
	);

	void busy_thread_job( u32 const in_thread_index, sys::time const in_time_limit );
	void free_thread_job( u32 const in_helper_thread_index );
	void helper_thread_job( u32 const in_helper_thread_index );

private:
	enum : u32 {
		cs_max_count = engine_thread_count - engine_free_threads_first,
		cv_max_count = engine_free_threads_count + 1,

		ignore_thread_count = 1,
		ignore_thread_mask =
			( 1 << engine_thread_window )
	};

private:
	struct query_data;

private:
	friend class resource_cook;
	friend class resource;

	void create_child_resources(
		query_info const* const in_queries,
		u32 const in_query_count,
		query_info const& in_callback_info
	);

	void execute_tasks(
		resource_cook* const in_cook,
		cook_task_info const* const in_tasks,
		u32 const in_task_count,
		cook_task_info const& in_callback
	);

	void finish_cook( resource_cook* const in_cook, query_result const in_result );

private:
	void process_task( query_data* const data );

	void push_cook_queries(
		query_info const* const in_queries,
		u32 const in_query_count,
		query_data* const in_query_data
	);

	void push_query_data( query_data* const in_query_data );

private:
	sys::mpsc_queue<query_data*> m_queues[engine_thread_count];
	sys::mpmc_queue<query_data*> m_helper_queue;
	sys::critical_section m_cs[cs_max_count];
	sys::conditional_variable m_cv[cv_max_count];

	temporal_allocator<64> m_temp_allocator;

	u32 m_thread_count;
	u32 m_helper_thread_count;

	bool volatile m_keep_processing;
};

extern resource_system g_resource_system;

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
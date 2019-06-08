#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED

#include <types.h>
#include <lib/temporal_allocator.h>
#include <system/critical_section.h>
#include <system/mpsc_queue.h>
#include <system/mpmc_queue.h>
#include <utils/engine_threads.h>

#include "resource_system_types.h"

namespace resource_system {

class resource_cook;

class resource_system
{
public:
	void create( u32 const in_thread_count );

	void stop( );
	void destroy( );
	
	void query_resources(
		resource_cook* const* const in_cooks,
		u32 const in_cook_count,
		user_query_callback const& in_callback,
		pointer const in_callback_data,
		uptr const in_callback_data_size
	);

	void busy_thread_job( u32 const in_thread_index, u64 const in_time_limit );
	void free_thread_job( u32 const in_helper_thread_index );
	void helper_thread_job( u32 const in_helper_thread_index );

private:
	enum : uptr {
		event_max_count = engine_thread_count - engine_free_threads_first
	};

private:
	struct query_data;

private:
	friend class resource_cook;

	void query_child_resources(
		resource_cook* const* const in_requested_cooks,
		u32 const in_requested_cook_count,
		resource_cook* const in_parent_cook,
		cook_functor const& in_functor,
		u32 const in_functor_thread_index
	);

	void finish_cook( resource_cook* const in_cook, query_result const in_result );

private:
	void process_task( query_data* const data );

	void push_cook_queries(
		resource_cook* const* const in_cooks,
		u32 const in_cook_count,
		query_data* const in_query_data
	);

	void push_query_data( query_data* const in_query_data );

private:
	sys::mpsc_queue<query_data*> m_queues[engine_thread_count];
	sys::mpmc_queue<query_data*> m_helper_queue;
	sys::critical_section m_thread_events[event_max_count];

	temporal_allocator<64> m_temp_allocator;

	u32 m_thread_count;
	u32 m_helper_thread_count;

	bool volatile m_keep_processing;
};

extern resource_system g_resource_system;

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
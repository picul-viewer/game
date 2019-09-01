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

class resource_system
{
public:
	void create( u32 const in_thread_count );
	void destroy( );

	void stop( );

	void create_tasks(
		task_info const* const in_parent,
		task_info const* const in_children,
		u32 const in_children_count,
		bool in_collect_children_results
	);

	void create_subtasks(
		task_info const* const in_parent,
		task_info const* const in_children,
		u32 const in_children_count,
		bool in_collect_children_results
	);

	void set_current_task_result(
		pvoid const in_result
	);

	void set_task_result(
		u32 const in_task_data_offset,
		u32 const in_result_index,
		pvoid const in_result
	);

	void busy_thread_job( sys::time const in_time_limit );
	void free_thread_job( );
	void helper_thread_job( );

private:
	enum : u32 {
		cs_max_count = engine_thread_count - engine_free_threads_first,
		cv_max_count = engine_free_threads_count + 1,

		ignore_thread_count = 1,
		ignore_thread_mask =
			( 1 << engine_thread_window )
	};

private:
	struct task_data;

private:
	bool has_tasks( ) const;

	void process_task( u32 const thread_index, task_data* const data );

	void push_task( task_data* const in_task_data );

private:
	friend class resource_cook;

	void get_current_result_data( u32& in_parent_data_offset, u32& in_result_index ) const;

private:
	struct thread_local_data
	{
		union
		{
			u8 align_helper[Cache_Line];
			struct
			{
				task_data* current_task_data;
			};
		};
	};

private:
	thread_local_data m_thread_local_data[engine_thread_count];
	sys::mpsc_queue<task_data*> m_queues[engine_thread_count];
	sys::mpmc_queue<task_data*> m_helper_queue;
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
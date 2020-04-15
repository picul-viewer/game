#ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
#define GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED

#include <types.h>
#include <lib/temporal_allocator.h>
#include <system/mpsc_queue.h>
#include <system/mpmc_queue.h>
#include <system/system_event.h>
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

	custom_task_context create_custom_tasks(
		u32 const in_count,
		task_info const& in_parent
	);

	custom_task_context create_custom_subtasks(
		u32 const in_count,
		task_info const* const in_parent
	);

	void set_current_task_result(
		pvoid const in_result
	);

	void set_task_result(
		u32 const in_task_data_offset,
		u32 const in_result_index,
		pvoid const in_result
	);

	void finish_custom_task(
		custom_task_context const in_context
	);


	bool process_task( u32 const in_thread_index );
	bool process_helper_task( );

	sys::system_event const& queue_event( u32 const in_thread_index ) const;
	sys::system_event const& helper_queue_event( ) const;

private:
	enum : u32 {
		ignore_thread_count = 1,
		ignore_thread_mask =
			( 1 << engine_thread_window )
	};

private:
	struct task_data;

private:
	bool queues_empty( ) const;
	bool all_asleep( ) const;

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
	sys::system_event m_events[engine_thread_count + 1];
	bool volatile m_keep_process[engine_thread_count];
	bool volatile m_is_sleeping[engine_thread_count];

	temporal_allocator<64> m_temp_allocator;

	u32 m_thread_count;
};

extern resource_system g_resource_system;

} // namespace resource_system

#endif // #ifndef GUARD_RESOURCE_SYSTEM_RESOURCE_SYSTEM_H_INCLUDED
#ifndef GUARD_UTILS_RESOURCE_SYSTEM_THREADS_H_INCLUDED
#define GUARD_UTILS_RESOURCE_SYSTEM_THREADS_H_INCLUDED

#include <types.h>

enum resource_system_threads : u32
{
	resource_system_busy_threads_first = 0,

	resource_system_thread_main = resource_system_busy_threads_first,
	resource_system_thread_window,

	resource_system_free_threads_first,

	resource_system_thread_fs = resource_system_free_threads_first,

	resource_system_helper_threads_first,

	resource_system_helper_thread_0 = resource_system_helper_threads_first,
	resource_system_helper_thread_1,
	resource_system_helper_thread_2,
	resource_system_helper_thread_3,
	resource_system_helper_thread_4,
	resource_system_helper_thread_5,
	resource_system_helper_thread_6,
	resource_system_helper_thread_7,
	resource_system_helper_thread_8,
	resource_system_helper_thread_9,
	resource_system_helper_thread_10,
	resource_system_helper_thread_11,
	resource_system_helper_thread_12,
	resource_system_helper_thread_13,
	resource_system_helper_thread_14,
	resource_system_helper_thread_15,

	// Different engine threads.

	resource_system_thread_count
};

#endif // #ifndef GUARD_UTILS_RESOURCE_SYSTEM_THREADS_H_INCLUDED
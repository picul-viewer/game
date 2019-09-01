#ifndef GUARD_UTILS_ENGINE_THREADS_H_INCLUDED
#define GUARD_UTILS_ENGINE_THREADS_H_INCLUDED

#include <types.h>
#include <system/thread.h>

enum engine_threads : u32
{
	engine_busy_threads_first = 0,

	engine_thread_main = engine_busy_threads_first,
	engine_thread_window,

	engine_free_threads_first,

	engine_thread_fs = engine_free_threads_first,

	engine_helper_threads_first,

	engine_helper_thread_0 = engine_helper_threads_first,
	engine_helper_thread_1,
	engine_helper_thread_2,
	engine_helper_thread_3,
	engine_helper_thread_4,
	engine_helper_thread_5,
	engine_helper_thread_6,
	engine_helper_thread_7,
	engine_helper_thread_8,
	engine_helper_thread_9,
	engine_helper_thread_10,
	engine_helper_thread_11,
	engine_helper_thread_12,
	engine_helper_thread_13,
	engine_helper_thread_14,
	engine_helper_thread_15,

	engine_thread_count,

	engine_busy_threads_count = engine_free_threads_first - engine_busy_threads_first,
	engine_free_threads_count = engine_helper_threads_first - engine_free_threads_first,
	engine_helper_threads_count = engine_thread_count - engine_helper_threads_first
};

namespace utils {

void set_thread_index( u32 const in_thread_index );
u32 thread_index( );

} // namespace utils

#endif // #ifndef GUARD_UTILS_ENGINE_THREADS_H_INCLUDED
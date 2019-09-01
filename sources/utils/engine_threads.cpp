#include "engine_threads.h"
#include <macros.h>

namespace utils {

thread_local u32 s_thread_index = engine_thread_count;

void set_thread_index( u32 const in_thread_index )
{
	ASSERT_CMP( in_thread_index, <, engine_thread_count );
	s_thread_index = in_thread_index;
}

u32 thread_index( )
{
	return s_thread_index;
}

} // namespace utils
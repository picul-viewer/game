#ifndef __fs_fs_core_h_included_
#define __fs_fs_core_h_included_

#include <core/types.h>
#include <core/std.h>
#include <core/threads.h>

namespace fs {

namespace fs_core
{
	typedef spsc_queue<procedure<256>> fs_queue;
	void create( );
	void destroy( );
	void wait_for_destruction( );

	fs_queue& get_fs_queue( );

	template<typename ... Args>
	void read_file( const char* file_name, void(*post_processing_functor)( pointer, uptr, Args ... ), Args ... post_processing_args );

	template<typename ... Args>
	void write_file( const char* file_name, pointer data, uptr size, void(*post_processing_functor)( pointer, uptr, Args ... ), Args ... post_processing_args );

} // namespace fs_core

bool file_exists( pcstr file_name );

} // namespace fs

#include "fs_core_inline.h"

#endif // #ifndef __fs_fs_core_h_included_
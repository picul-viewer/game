#ifndef __fs_fs_core_h_included_
#define __fs_fs_core_h_included_

#include <core/types.h>
#include <core/std.h>
#include <core/threads.h>

namespace fs {

namespace core
{
	typedef task_queue<256> fs_queue;
	void create( );
	void destroy( );
	void wait_for_destruction( );

	fs_queue& get_fs_queue( );

	template<typename ... Args>
	void read_file( const char* file_name, void(*callback)( pointer, uptr, Args ... ), Args ... args );
	template<typename T, typename ... Args>
	void read_file( const char* file_name, void(T::*callback)( pointer, uptr, Args ... ), T* this_pointer, Args ... args );
	template<typename T, typename ... Args>
	void read_file( const char* file_name, void(T::*callback)( pointer, uptr, Args ... ) const, T const* this_pointer, Args ... args );
	
	template<typename ... Args>
	void write_file( const char* file_name, pointer data, uptr size, void(*callback)( pointer, uptr, Args ... ), Args ... args );
	template<typename T, typename ... Args>
	void write_file( const char* file_name, pointer data, uptr size, void(T::*callback)( pointer, uptr, Args ... ), T* this_pointer, Args ... args );
	template<typename T, typename ... Args>
	void write_file( const char* file_name, pointer data, uptr size, void(T::*callback)( pointer, uptr, Args ... )const, T const* this_pointer, Args ... args );

} // namespace core

} // namespace fs

#include "fs_core_inline.h"

#endif // #ifndef __fs_fs_core_h_included_
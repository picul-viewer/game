#ifndef __fs_fs_core_inline_h_included_
#define __fs_fs_core_inline_h_included_

#include <core/macros.h>
#include <core/std.h>
#include <Windows.h>

namespace fs {

namespace core {

template<typename Functor, typename ... Args>
void read_file_impl( const char* file_name, Functor callback, Args ... args )
{
	HANDLE file = CreateFile( file_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );

#ifdef DEBUG
	if ( file == INVALID_HANDLE_VALUE )
	{
		LOG( "unable to open file for reading: %s\n", file_name );

		callback( pointer( nullptr ), (uptr)0, args ... );
		return;
	}
#endif // #ifdef DEBUG
	
	DWORD file_size = GetFileSize( file, nullptr );
	
	if ( file_size == 0 )
	{
		LOG( "specified file is empty: %s\n", file_name );
		CloseHandle( file );

		callback( pointer( nullptr ), (uptr)0, args ... );
		return;
	}

	pointer data = mem_allocator( ).allocate( (uptr)file_size );

	DWORD bytes_read = 0;
	ReadFile( file, data, file_size, &bytes_read, nullptr );
	
#ifdef DEBUG
	if ( bytes_read != file_size )
	{
		LOG( "unable to read from file: %s\n", file_name );
		CloseHandle( file );

		callback( pointer( nullptr ), (uptr)0, args ... );
		return;
	}
#endif // #ifdef DEBUG

	CloseHandle( file );

	callback( data, (uptr)file_size, args ... );
}

template<typename ... Args>
void read_file( const char* file_name, void(*callback)( pointer, uptr, Args ... ), Args ... args )
{
	get_fs_queue( ).push( read_file_impl, file_name, make_functor( callback ), args ... );
}

template<typename T, typename ... Args>
void read_file( const char* file_name, void(T::*callback)( pointer, uptr, Args ... ), T* this_pointer, Args ... args )
{
	get_fs_queue( ).push( read_file_impl, file_name, make_member_functor( callback, this_pointer ), args ... );
}

template<typename T, typename ... Args>
void read_file( const char* file_name, void(T::*callback)( pointer, uptr, Args ... ) const, T const* this_pointer, Args ... args )
{
	get_fs_queue( ).push( read_file_impl, file_name, make_member_functor( callback, this_pointer ), args ... );
}

template<typename Functor, typename ... Args>
void write_file_impl( const char* file_name, pointer data, uptr size, Functor callback, Args ... args )
{
	ASSERT( data != nullptr );
	ASSERT( size != 0 );

	HANDLE file = CreateFile( file_name, GENERIC_WRITE | DELETE, 0, nullptr, CREATE_ALWAYS, 0, nullptr );

#ifdef DEBUG
	if ( file == INVALID_HANDLE_VALUE )
	{
		LOG( "unable to open file for writing: %s\n", file_name );
		
		callback( pointer( nullptr ), (uptr)0, args ... );
		return;
	}
#endif // #ifdef DEBUG

	DWORD bytes_write = 0;
	WriteFile( file, data, size, &bytes_write, nullptr );
	
#ifdef DEBUG
	if ( bytes_write != file_size )
	{
		LOG( "unable to write to file: %s\n", file_name );
		CloseHandle( file );
		
		callback( pointer( nullptr ), (uptr)0, args ... );
		return;
	}
#endif // #ifdef DEBUG

	CloseHandle( file );

	callback( data, (uptr)size, args ... );
}

template<typename ... Args>
void write_file( const char* file_name, pointer data, uptr size, void(*callback)( pointer, uptr, Args ... ), Args ... args )
{
	get_fs_queue( ).push( write_file_impl, file_name, data, size, make_functor( callback ), args ... );
}

template<typename T, typename ... Args>
void write_file( const char* file_name, pointer data, uptr size, void(T::*callback)( pointer, uptr, Args ... ), T* this_pointer, Args ... args )
{
	get_fs_queue( ).push( write_file_impl, file_name, data, size, make_member_functor( callback, this_pointer ), args ... );
}

template<typename T, typename ... Args>
void write_file( const char* file_name, pointer data, uptr size, void(T::*callback)( pointer, uptr, Args ... )const, T const* this_pointer, Args ... args )
{
	get_fs_queue( ).push( write_file_impl, file_name, data, size, make_member_functor( callback, this_pointer ), args ... );
}

} // namespace core

} // namespace fs

#endif // #ifndef __fs_fs_core_inline_h_included_
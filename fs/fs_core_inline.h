#ifndef __fs_fs_core_inline_h_included_
#define __fs_fs_core_inline_h_included_

#include <core/macros.h>
#include <core/structs.h>
#include <Windows.h>

namespace fs {

namespace fs_core {

template<typename ... Args>
void read_file_impl( const char* file_name, void(*post_processing_functor)( pointer, uptr, Args ... ), Args ... post_processing_args )
{
	HANDLE file = CreateFile( file_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );

#ifdef DEBUG
	if ( file == nullptr )
	{
		LOG( "unable to open file for reading: %s\n", file_name );
		return;
	}
#endif // #ifdef DEBUG

	DWORD file_size;
	GetFileSize( file, &file_size );
	
#ifdef DEBUG
	if ( file_size == 0 )
	{
		LOG( "unable to read from empty file: %s\n", file_name );
		CloseHandle( file );
		return;
	}
#endif // #ifdef DEBUG

	pointer data = mem_allocator::allocate( (uptr)file_size );

	DWORD bytes_read = 0;
	ReadFile( file, data, file_size, &bytes_read, nullptr );
	
#ifdef DEBUG
	if ( bytes_read != file_size )
	{
		LOG( "unable to read from file: %s\n", file_name );
		CloseHandle( file );
		return;
	}
#endif // #ifdef DEBUG

	CloseHandle( file );

	post_processing_functor( data, file_size, post_processing_args ... );
}

template<typename ... Args>
void read_file( const char* file_name, void(*post_processing_functor)( pointer, uptr, Args ... ), Args ... post_processing_args )
{
	get_fs_queue( ).push( read_file_impl, file_name, post_processing_functor, post_processing_args ... );
}

template<typename ... Args>
void write_file_impl( const char* file_name, pointer data, uptr size, void(*post_processing_functor)( pointer, uptr, Args ... ), Args ... post_processing_args )
{
	HANDLE file = CreateFile( file_name, GENERIC_WRITE | DELETE, 0, nullptr, CREATE_ALWAYS, 0, nullptr );

#ifdef DEBUG
	if ( file == nullptr )
	{
		LOG( "unable to open file for writing: %s\n", file_name );
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
		return;
	}
#endif // #ifdef DEBUG

	CloseHandle( file );

	post_processing_functor( data, size, post_processing_args ... );
}

template<typename ... Args>
void write_file( const char* file_name, pointer data, uptr size, void(*post_processing_functor)( pointer, uptr, Args ... ), Args ... post_processing_args )
{
	get_fs_queue( ).push( write_file_impl, file_name, data, size, post_processing_functor, post_processing_args ... );
}

} // namespace fs_core

} // namespace fs

#endif // #ifndef __fs_fs_core_inline_h_included_
#include "fs_resource_system_worker.h"

#include <Windows.h>

namespace fs {

void fs_resource_system_worker::thread_func( void* arg )
{
	fs_resource_system_worker* this_ptr = (fs_resource_system_worker*)arg;

	this_ptr->work( );
}

void fs_resource_system_worker::work( )
{
	query_queue::value_type query;

	while ( m_keep_alive )
	{
		m_queue.pop( query );
		fs_resource_system_worker::read_impl( query );
	}

	while ( !m_queue.empty( ) )
	{
		m_queue.pop( query );
		fs_resource_system_worker::read_impl( query );
	}
}

void fs_resource_system_worker::create( )
{
	m_queue.create	( 1 * Mb );

	m_keep_alive	= true;

	m_thread.create	( thread_func, 0, this );
}

void fs_resource_system_worker::destroy( )
{
	m_keep_alive	= false;
}

void fs_resource_system_worker::wait_for_destruction( )
{
	m_thread.destroy( INFINITE );
}

void fs_resource_system_worker::read( pcstr filename, callback_type const& callback )
{
	m_queue.push( { filename, callback } );
}

void fs_resource_system_worker::read_impl( query query )
{
	HANDLE file = CreateFile( query.filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );

#ifdef DEBUG
	if ( file == nullptr )
	{
		LOG( "unable to open file for reading: %s\n", query.filename );
		return;
	}
#endif // #ifdef DEBUG

	DWORD file_size;
	GetFileSize( file, &file_size );
	
#ifdef DEBUG
	if ( file_size == 0 )
	{
		LOG( "unable to read from empty file: %s\n", query.filename );
		CloseHandle( file );
		return;
	}
#endif // #ifdef DEBUG

	pointer data = mem_allocator( ).allocate( (uptr)file_size );

	DWORD bytes_read = 0;
	ReadFile( file, data, file_size, &bytes_read, nullptr );
	
#ifdef DEBUG
	if ( bytes_read != file_size )
	{
		LOG( "unable to read from file: %s\n", query.filename );
		CloseHandle( file );
		return;
	}
#endif // #ifdef DEBUG

	CloseHandle( file );

	query.callback( data, file_size );
}

} // namespace fs
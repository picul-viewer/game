#include "request_ptr.h"
#include <macros.h>
#include <lib/allocator.h>
#include <system/file.h>
#include "resource_system.h"
#include "query_data.h"

namespace resource_system {

query_result request_ptr::load( pcstr const in_file_path, u32 const in_initial_ref_count )
{
	sys::file f;
	f.open( in_file_path, sys::file::open_read );
	ASSERT( f.is_valid( ) );

	uptr const file_size = f.size( );
	ASSERT_CMP( file_size, <, 0x100000000 );

	u32 size_to_allocate;
	f.read( &size_to_allocate, sizeof(u32) );

	u32 const request_size = (u32)( file_size - sizeof(u32) );
	ASSERT_CMP( size_to_allocate, >, request_size );

	request_header* const header = std_allocator( ).allocate( size_to_allocate );

	header->ref_count = in_initial_ref_count;
	f.read( &header->finish_query_offset, request_size );

	f.close( );

	return (query_result)header;
}

request_ptr::request_ptr( pointer const in_data ) :
	m_data( in_data )
{ }

request_ptr::request_ptr( query_result const in_query_result ) :
	m_data( (pvoid)in_query_result )
{ }

request_ptr::request_ptr( request_ptr const& in_other ) :
	m_data( in_other.m_data )
{
	mt_u32& ref_counter = ( (request_header*)m_data )->ref_count;
	interlocked_inc( ref_counter );
}

request_ptr& request_ptr::operator=( request_ptr const& in_other )
{
	m_data = in_other.m_data;
	mt_u32& ref_counter = ( (request_header*)m_data )->ref_count;
	interlocked_inc( ref_counter );
	return *this;
}

request_ptr::request_ptr( request_ptr&& in_other )
{
	ASSERT( m_data == nullptr );
	m_data = in_other.m_data;
	in_other.m_data = nullptr;
}

request_ptr& request_ptr::operator=( request_ptr&& in_other )
{
	std::swap( m_data, in_other.m_data );
	return *this;
}

request_ptr::~request_ptr( )
{
	if ( m_data == nullptr )
		return;

	mt_u32& ref_counter = ( (request_header*)m_data )->ref_count;
	if ( interlocked_dec( ref_counter ) == 0 )
	{
		std_allocator( ).deallocate( m_data );
	}
}

void request_ptr::submit( query_callback_id const in_callback_id )
{
	request_header* const header = m_data;
	
	// Share ownership with resource system.
	mt_u32& ref_counter = header->ref_count;
	interlocked_inc( ref_counter );
	
	query_data* const query = m_data + header->finish_query_offset;

	query_callback_id& callback = query->callback_id( );
	callback = in_callback_id;

	g_resource_system.manage_query( query );
}

void request_ptr::release_by_pointer( pointer const in_request )
{
	// Resource system does not own request for now.
	mt_u32& ref_counter = ( (request_header*)in_request )->ref_count;
	if ( interlocked_dec( ref_counter ) == 0 )
	{
		std_allocator( ).deallocate( in_request );
	}
}

} // namespace resource_system
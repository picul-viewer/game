#include "file_ptr.h"
#include <macros.h>
#include <lib/allocator.h>
#include <system/file.h>
#include "query_data.h"

namespace resource_system {

query_result file_ptr::load( pcstr const in_file_path, u32 const in_initial_ref_count )
{
	sys::file f;
	f.open( in_file_path, sys::file::open_read );
	ASSERT( f.is_valid( ) );

	uptr const file_size = f.size( );
	ASSERT_CMP( file_size, <, 0x100000000 );

	uptr const size = file_header::calculate_size( file_size );

	file_header* const header = std_allocator( ).allocate( size );

	header->ref_count = in_initial_ref_count;
	header->size = (u32)file_size;
	f.read( header->data, file_size );

	f.close( );

	return (query_result)header;
}

file_ptr::file_ptr( query_result const in_query_result ) :
	m_data( (pvoid*)in_query_result )
{ }

file_ptr::file_ptr( file_ptr const& in_other ) :
	m_data( in_other.m_data )
{
	mt_u32& ref_counter = ( (file_header*)m_data )->ref_count;
	interlocked_inc( ref_counter );
}

file_ptr& file_ptr::operator=( file_ptr const& in_other )
{
	m_data = in_other.m_data;
	mt_u32& ref_counter = ( (file_header*)m_data )->ref_count;
	interlocked_inc( ref_counter );
	return *this;
}

file_ptr::file_ptr( file_ptr&& in_other )
{
	ASSERT( m_data == nullptr );
	m_data = in_other.m_data;
	in_other.m_data = nullptr;
}

file_ptr& file_ptr::operator=( file_ptr&& in_other )
{
	std::swap( m_data, in_other.m_data );
	return *this;
}

file_ptr::~file_ptr( )
{
	if ( m_data == nullptr )
		return;

	mt_u32& ref_counter = ( (file_header*)m_data )->ref_count;
	if ( interlocked_dec( ref_counter ) == 0 )
	{
		std_allocator( ).deallocate( m_data );
	}
}

pointer file_ptr::data( ) const
{
	ASSERT( m_data );
	return ( (file_header*)m_data )->data;
}

u32 file_ptr::size( ) const
{
	ASSERT( m_data );
	return ( (file_header*)m_data )->size;
}

} // namespace resource_system
#include "raw_data.h"
#include <macros.h>
#include <lib/algorithms.h>
#include <lib/allocator.h>
#include <lib/strings.h>
#include <system/file.h>

namespace resource_system {

DEFINE_DEFAULT_RESOURCE_HANDLE( raw_data, raw_data*, nullptr )


void raw_data::delete_resource( )
{
	std_allocator( ).deallocate( this );
}

raw_data* raw_data::from_handle( raw_data_handle const& in_handle )
{
	return in_handle;
}

raw_data_handle raw_data::to_handle( raw_data* const in_resource )
{
	return in_resource;
}

raw_data* raw_data::to_handle_value( raw_data* const in_resource )
{
	return in_resource;
}

pointer raw_data::data( ) const
{
	return m_data;
}

uptr raw_data::size( ) const
{
	return m_size;
}


raw_data_cook* raw_data_cook::create( pcstr const in_path )
{
	uptr const size = strings::length( in_path ) + 1;

	raw_data_cook* const result = std_allocator( ).allocate( size );
	result->init( );
	strings::copy_n( result->m_path, in_path, size );

	return result;
}

void raw_data_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void raw_data_cook::create_resource( )
{
	sys::file f;
	f.create( m_path, sys::file::open_read );

	if ( !f.is_valid( ) )
	{
		finish( nullptr );
		return;
	}

	uptr const raw_data_size = f.size( );
	ASSERT_CMP( raw_data_size, <, 0x100000000 );

	uptr const size = sizeof(raw_data) + raw_data_size;

	raw_data* const data = std_allocator( ).allocate( size );

	data->m_size = raw_data_size;
	f.read( data->m_data, raw_data_size );

	f.destroy( );

	finish( data );
}

} // namespace resource_system
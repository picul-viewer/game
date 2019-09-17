#include "raw_data.h"
#include <macros.h>
#include <lib/allocator.h>
#include <lib/strings.h>
#include <system/file.h>

namespace resources {

raw_data* raw_data::create( uptr const size )
{
	uptr const allocation_size = sizeof(raw_data) + size;
	raw_data* const result = std_allocator( ).allocate( allocation_size );
	result->m_size = size;
	return result;
}

void raw_data::destroy( )
{
	std_allocator( ).deallocate( this );
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

	raw_data_cook* const result = std_allocator( ).allocate( sizeof(raw_data_cook) + size );
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

	uptr const file_size = f.size( );
	raw_data* const data = raw_data::create( file_size );

	f.read( data->data( ), file_size );

	f.destroy( );

	finish( data );
}

} // namespace resources
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


raw_data_cook::raw_data_cook( pcstr const in_path )
{
#ifdef DEBUG
	{
		// Try to open file to catch missing file error.
		sys::file f;
		f.create( in_path, sys::file::open_read );
		f.destroy( );
	}
#endif // #ifdef DEBUG

	uptr const size = strings::length( in_path ) + 1;
	strings::copy_n( m_path, in_path, size );
}

uptr raw_data_cook::size( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	return sizeof(raw_data_cook) + length + 1;
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
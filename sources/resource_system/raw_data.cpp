#include "raw_data.h"
#include <lib/allocator.h>
#include <macros.h>
#include <system/file.h>

namespace resource_system {

u32 raw_data::add_ref( )
{
	return interlocked_inc( m_ref_count );
}

u32 raw_data::release( )
{
	u32 const ref_count = interlocked_dec( m_ref_count );

	if ( ref_count == 0 )
		cook_type::destroy_resource( this );

	return ref_count;
}

pointer raw_data::data( ) const
{
	return m_data;
}

u32 raw_data::size( ) const
{
	return m_size;
}


raw_data_cook::raw_data_cook( ) :
	resource_cook( resource_type_raw_data )
{ }

raw_data_cook::raw_data_cook( pcstr const in_path ) :
	raw_data_cook( )
{
	m_path = in_path;
}

void raw_data_cook::create_resource( )
{
	sys::file f;
	f.create( m_path, sys::file::open_read );
	ASSERT( f.is_valid( ) );

	uptr const raw_data_size = f.size( );
	ASSERT_CMP( raw_data_size, <, 0x100000000 );

	uptr const size = sizeof(raw_data) + raw_data_size;

	raw_data* const data = std_allocator( ).allocate( size );

	data->m_ref_count = 1;
	data->m_size = (u32)raw_data_size;
	f.read( data->m_data, raw_data_size );

	f.destroy( );

	finish( (query_result)data );
}

void raw_data_cook::destroy_resource( raw_data* const in_resource )
{
	std_allocator( ).deallocate( in_resource );
}

} // namespace resource_system
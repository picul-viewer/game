#include "texture_cook.h"

#include <lib/allocator.h>
#include <lib/hash.h>
#include <lib/reader.h>
#include <lib/strings.h>
#include <resources/raw_data.h>

#include "dds_loader.h"
#include "texture.h"

namespace render {

texture_cook* texture_cook::create( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	uptr const size = sizeof(texture_cook) + length + 1;

	texture_cook* const result = std_allocator( ).allocate( sizeof(texture_cook) + size );

	u32 const id = lib::hash32( in_path, length );
	result->init( id );

	result->m_length = length;
	strings::copy_n( result->m_path, in_path, size );

	return result;
}

void texture_cook::destroy( pointer const in_cook )
{
	std_allocator( ).deallocate( in_cook );
}

void texture_cook::create_resource( )
{
	check_resource_exists( &texture_cook::query_file );
}

void texture_cook::query_file( texture* const in_resource_ptr )
{
	m_result = in_resource_ptr;

	raw_data_cook* const cook = raw_data_cook::create( m_path );

	create_child_resources( callback_task<&texture_cook::on_file_loaded>( ), cook );
}

void texture_cook::on_file_loaded( queried_resources& in_queried )
{
	raw_data::ptr const data = in_queried.get_resource<raw_data::ptr>( );
	ASSERT( data != nullptr );

	lib::reader r( data->data( ), data->size( ) );

	ID3D11ShaderResourceView* texture;
	dds_loader::load( texture, r );
	m_result->m_view.set( texture );

	finish( m_result );
}

} // namespace render
#include "font_cook.h"
#include "font.h"
#include <lib/memory.h>
#include <render/texture_cook.h>
#include <resources/raw_data.h>
#include <resources/resources_path.h>

namespace ui {

font_cook::font_cook( pcstr const in_path ) :
	shared_resource_cook( create( in_path ) )
{ }

u32 font_cook::create( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	uptr const size = length + 1;

	m_length = length;
	strings::copy_n( m_path, in_path, size );

	return lib::hash32( in_path, length );
}

uptr font_cook::size( pcstr const in_path )
{
	uptr const length = strings::length( in_path );
	return sizeof(font_cook) + length + 1;
}

void font_cook::create_resource( )
{
	check_resource_exists( &font_cook::query_file );
}

void font_cook::query_file( font* const in_resource_ptr )
{
	m_result = in_resource_ptr;

	raw_data_cook* const cook = create_cook<raw_data_cook>( m_path );

	create_child_resources( callback_task<&font_cook::on_file_loaded>( ), cook );
}

void font_cook::on_file_loaded( queried_resources& in_queried )
{
	raw_data::ptr const data = in_queried.get_resource<raw_data::ptr>( );
	ASSERT( data != nullptr );

	lib::reader r( data->data( ), data->size( ) );

	pcstr const texture_path = r.read_str( );

	m_result->m_texture_dimensions.x = r.read<u16>( );
	m_result->m_texture_dimensions.y = r.read<u16>( );

	m_result->m_chars_in_row = r.read<u8>( );
	m_result->m_chars_in_column = r.read<u8>( );
	m_result->m_char_height = r.read<u8>( );
	m_result->m_start_char = r.read<u8>( );
	m_result->m_char_count = r.read<u8>( );

	pvoid const chars_width = r.read_data( m_result->m_char_count );
	memory::copy( m_result->m_chars_width.data( ), chars_width, sizeof(u8) * m_result->m_char_count );

	render::texture_cook* const texture_cook = create_cook<render::texture_cook>( get_resource_path( texture_path ).c_str( ) );

	create_child_resources( callback_task<&font_cook::on_texture_ready>( ), texture_cook );
}

void font_cook::on_texture_ready( queried_resources& in_queried )
{
	m_result->m_texture = lib::move( in_queried.get_resource<render::texture_handle>( ) );
	ASSERT( m_result->m_texture != render::texture_handle::invalid );

	finish( m_result );
}

} // namespace ui
#include "font.h"
#include <macros.h>
#include <lib/memory.h>
#include <render/ui_batch.h>
#include <render/world.h>
#include <utils/resources_path.h>

namespace ui {

void font::create( binary_config& in_config )
{
	pcstr const texture_path = in_config.read_str( );
	m_texture = render::g_world.create_texture( utils::get_resource_path( texture_path ).c_str( ) );

	m_texture_dimensions.x = in_config.read<u16>( );
	m_texture_dimensions.y = in_config.read<u16>( );

	m_chars_in_row = in_config.read<u8>( );
	m_chars_in_column = in_config.read<u8>( );
	m_char_height = in_config.read<u8>( );
	m_start_char = in_config.read<u8>( );
	m_char_count = in_config.read<u8>( );

	pvoid chars_width = in_config.read_data( m_char_count );
	memory::copy( m_chars_width.data( ), chars_width, sizeof(u8) * m_char_count );
}

void font::destroy( )
{
	render::g_world.destroy_texture( m_texture );
}

u8 font::get_char_width( char const in_char ) const
{
	ASSERT( ( in_char >= m_start_char ) && ( in_char < m_start_char + m_char_count ) );
	return m_chars_width[in_char - m_start_char];
}

u32 font::get_string_width( pcstr const in_str ) const
{
	uptr const length = strings::length( in_str );

	return get_string_width( in_str, length );
}

u32 font::get_string_width( pcstr const in_str, uptr const in_length ) const
{
	pcstr s = in_str;
	u32 width = 0;

	for ( uptr l = in_length; l; ++s, --l )
		width += get_char_width( *s );

	return width;
}

void font::render_string(
	pcstr const caption, uptr const length, math::u16x2 const position,
	u16 const font_size, math::half4 const color, render::ui_batch& batch ) const
{
	pcstr s = caption;
	uptr l = length;

	u16 offset = 0;

	for ( ; l; ++s, --l )
	{
		char const c = *s;

		if ( ( c < m_start_char ) || ( c >= m_start_char + m_char_count ) )
			continue;

		u8 const index = c - m_start_char;
		
		u8 const width = m_chars_width[index];
		u16 const next_offset = offset + width;

		// Filter off invisible characters.
		if ( c > ' ' )
		{
			u8 const col_index = index % m_chars_in_row;
			u8 const row_index = index / m_chars_in_row;

			math::u16x4 const corners_position( 
				position.x + offset * font_size / m_char_height,
				position.y - font_size,
				position.x + next_offset * font_size / m_char_height,
				position.y
			);
		
			math::u16x4 const corners_texcoord(
				65535ul * col_index / m_chars_in_column,
				65535ul * row_index / m_chars_in_row + 65535ul * m_char_height / m_texture_dimensions.y,
				65535ul * col_index / m_chars_in_column + 65535ul * width / m_texture_dimensions.x,
				65535ul * row_index / m_chars_in_row
			);

			batch.add_quad( corners_position, m_texture, corners_texcoord, color );
		}

		offset = next_offset;
	}
}

void font::render_multiline_string(
	pcstr const caption, uptr const length, math::u16x2 const position, u16 const line_interval,
	u16 const font_size, math::half4 const color, render::ui_batch& batch ) const
{
	pcstr str = caption;
	pcstr const str_end = caption + length;

	u16 position_y = position.y;

	while ( str < str_end )
	{
		uptr const find_result = strings::find( str, '\n' );
		uptr const line_end = ( find_result != (uptr)-1 ) ? find_result : str_end - str;

		render_string( str, line_end, math::u16x2( position.x, position_y ), font_size, color, batch );

		str += line_end + 1;
		position_y -= font_size + line_interval;
	}
}

} // namespace ui
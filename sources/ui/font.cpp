#include "font.h"
#include <macros.h>
#include <lib/memory.h>
#include <lib/strings.h>
#include <render/ui_batch.h>
#include <render/world.h>

namespace ui {

DEFINE_SHARED_RESOURCE_HANDLE( font, u8, -1 )

void font::destroy_resource( font* const in_resource )
{
	in_resource->m_texture.release( );

	container( ).remove( in_resource );
}

u8 font::get_char_width( char const in_char ) const
{
	u8 const c = (u8)in_char;
	ASSERT( ( c >= m_start_char ) && ( c < m_start_char + m_char_count ) );
	return m_chars_width[c - m_start_char];
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
	u16 const fsize = ( font_size != 0 ) ? font_size : m_char_height;

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
				position.x + offset * fsize / m_char_height,
				position.y,
				position.x + next_offset * fsize / m_char_height,
				position.y + fsize
			);
		
			math::u16x4 const corners_texcoord(
				65535ul * col_index / m_chars_in_column,
				65535ul * row_index / m_chars_in_row,
				65535ul * col_index / m_chars_in_column + 65535ul * width / m_texture_dimensions.x,
				65535ul * row_index / m_chars_in_row + 65535ul * m_char_height / m_texture_dimensions.y
			);

			batch.add_quad(
				corners_position, corners_texcoord,
				math::half4( 0.0f, 0.0f, 0.0f, color.w ),
				math::half4( color.vx, 0.0f ),
				m_texture );
		}

		offset = next_offset;
	}
}

void font::render_multiline_string(
	pcstr const caption, uptr const length, math::u16x2 const position, u16 const in_line_interval,
	u16 const font_size, math::half4 const color, render::ui_batch& batch ) const
{
	u16 const fsize = ( font_size != 0 ) ? font_size : m_char_height;

	pcstr str = caption;
	pcstr const str_end = caption + length;

	u16 position_y = position.y;

	while ( str < str_end )
	{
		uptr const find_result = strings::find( str, '\n' );
		uptr const line_end = ( find_result != (uptr)-1 ) ? find_result : str_end - str;

		render_string( str, line_end, math::u16x2( position.x, position_y ), fsize, color, batch );

		str += line_end + 1;
		position_y += fsize + in_line_interval;
	}
}

} // namespace ui
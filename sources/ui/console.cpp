#include "console.h"
#include <Windows.h>
#include <lib/allocator.h>
#include <system/file.h>
#include <system/window_input.h>
#include <render/ui_batch.h>

namespace ui {

void console::create( pcstr const in_font_path, u8 const in_visible_lines )
{
	sys::file f( in_font_path, sys::file::open_read );
	ASSERT( f.is_valid( ) );
	uptr const size = f.size( );
	pvoid const data = stack_allocate( size );
	f.read( data, size );
	f.close( );

	m_font.create( lib::reader( data, size ) );

	m_text_edit.create( m_text_edit_memory, text_edit_memory_size );

	m_text.create( );
	m_next_line_indices.create( nullptr, 64 * Kb );
	m_next_line_indices.push_back( (u32)-1 );

	m_callback = nullptr;
	m_border_size = 0;
	m_last_line_index = 0;
	m_font_size = m_font.get_font_size( );
	m_scroll_speed_in_lines = 1;
}

void console::set_default_colors( )
{
	m_background_color		= math::half4( 0.3f, 0.3f, 0.3f, 0.85f );
	m_edit_background_color	= math::half4( 0.15f, 0.15f, 0.15f, 0.85f );
	m_text_color			= math::half4( 0.6f, 0.6f, 0.6f, 0.85f );
	m_edit_text_color		= math::half4( 0.8f, 0.8f, 0.8f, 0.85f );
	m_selection_color		= math::half4( 0.45f, 0.45f, 0.45f, 0.85f );
	m_cursor_color			= math::half4( 1.0f, 1.0f, 1.0f, 0.85f );
}

void console::destroy( )
{
	m_font.destroy( );
	m_text.destroy( );
	m_text_edit.destroy( );
	m_next_line_indices.destroy( );
}
	
void console::on_char( u32 const in_key )
{
	using namespace sys;

	if ( in_key == (u32)key::enter )
	{
		if ( m_text_edit.get_length( ) == 0 )
			return;

		printf( "%s\n", m_text_edit_memory );

		if ( m_callback )
			m_callback( m_text_edit_memory );

		m_text_edit.clear( );

		return;
	}

	m_text_edit.on_char( in_key );
}

void console::on_input( )
{
	using namespace sys;

	RAWKEYBOARD const& keyboard = ( (RAWINPUT*)g_window_input.get_raw_input( ) )->data.keyboard;

	bool const key_down = ( keyboard.Flags & 0x1 ) == 0;

	if ( key_down )
	{
		key const k = (key)keyboard.VKey;

		switch ( k )
		{
			case key::page_up:
			{
				m_last_line_index = math::max( (u16)( m_last_line_index - m_scroll_speed_in_lines ), (u16)m_visible_lines_count );
				return;
			}
			case key::page_down:
			{
				m_last_line_index = math::min( (u16)( m_last_line_index + m_scroll_speed_in_lines ), (u16)( m_next_line_indices.size( ) - 1 ) );
				return;
			}
		}
	}

	m_text_edit.on_input( );
}

void console::render( render::ui_batch& in_batch ) const
{
	in_batch.add_color_quad( m_background_position, m_background_color );
	in_batch.add_color_quad( m_edit_background_position, m_edit_background_color );
	in_batch.next_level( );

	pcstr const edit_text = m_text_edit.get_text( );
	uptr const edit_length = m_text_edit.get_length( );

	uptr const selection_start = m_text_edit.get_selection_start( );
	ptr const selection_length = m_text_edit.get_selection_length( );

	u16 const selection_start_width = m_font.get_string_width( edit_text, selection_start );
	u16 const selection_length_width = m_font.get_string_width( edit_text, selection_start + selection_length );

	u16 const selection_left = math::min( selection_start_width, selection_length_width );
	u16 const selection_right = math::max( selection_start_width, selection_length_width );

	math::u16x4 const selection_position = math::u16x4(
		m_edit_text_position.x + selection_left, m_edit_text_position.y,
		m_edit_text_position.x + selection_right, m_edit_text_position.y + m_font_size );

	math::u16x4 const cursor_position = math::u16x4(
		m_edit_text_position.x + selection_length_width, m_edit_text_position.y,
		m_edit_text_position.x + selection_length_width + 1, m_edit_text_position.y + m_font_size );

	if ( selection_length )
		in_batch.add_color_quad( selection_position, m_selection_color );

	in_batch.add_color_quad( cursor_position, m_cursor_color );

	in_batch.next_level( );

	if ( m_next_line_indices.size( ) )
	{
		u16 text_offset = m_font_size * ( m_visible_lines_count - 1 );

		for ( u32 i = m_last_line_index; ( i > 0 ) && ( m_last_line_index - i < m_visible_lines_count ); --i )
		{
			u32 const index_start = m_next_line_indices[i - 1] + 1;
			u32 const index_end = m_next_line_indices[i];

			pcstr const text = m_text.data( ) + index_start;
			uptr const length = index_end - index_start;

			m_font.render_string( text, length, m_text_position + math::u16x2( 0, text_offset ), m_font_size, m_text_color, in_batch );
			text_offset -= m_font_size;
		}
	}

	m_font.render_string( edit_text, edit_length, m_edit_text_position, m_font_size, m_edit_text_color, in_batch );

	in_batch.next_level( );
}

void console::print( pcstr const in_string, uptr const in_length )
{
	pstr const line_start = m_text.allocate( in_length );
	strings::copy_n( line_start, in_string, in_length );

	update_line_endings( line_start, in_length );
}

void console::update_line_endings( pcstr const in_new_chars, uptr const in_length )
{
	pcstr chars = in_new_chars;
	uptr const end_position = m_text.data_end( ) - m_text.data( );

	for ( uptr i = in_length; i != 0; --i, ++chars )
	{
		if ( *chars == '\n' )
			m_next_line_indices.push_back( (u32)( end_position - i ) );
	}

	m_last_line_index = (u16)( m_next_line_indices.size( ) - 1 );
}

void console::set_corners_position( math::u16x4 const in_value )
{
	u16 const height = in_value.w - in_value.y;

	ASSERT_CMP( height, >, 4 * m_border_size + 2 * m_font_size );

	u16 const unused_height = ( height - 4 * m_border_size ) % m_font_size;
	// Top alignment.
	math::u16x4 const actual_value = math::u16x4( in_value.x, in_value.y, in_value.z, in_value.w - unused_height );
	
	m_visible_lines_count = ( actual_value.w - actual_value.y - 4 * m_border_size ) / m_font_size - 1;
	
	m_background_position = math::u16x4( actual_value.x, actual_value.y, actual_value.z, actual_value.w - ( 2 * m_border_size + m_font_size ) );
	m_edit_background_position = math::u16x4( actual_value.x, m_background_position.w, actual_value.z, actual_value.w );
	m_text_position = m_background_position.vx.vx + m_border_size;
	m_edit_text_position = m_edit_background_position.vx.vx + m_border_size;
}

} // namespace ui
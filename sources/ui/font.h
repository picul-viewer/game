#ifndef GUARD_UI_FONT_H_INCLUDED
#define GUARD_UI_FONT_H_INCLUDED

#include <types.h>
#include <lib/reader.h>
#include <lib/fixed_array.h>
#include <math/vector.h>
#include <render/resource_ids.h>

namespace render {
class ui_batch;
} // namespace render

namespace ui {

class font
{
public:
	void create( lib::reader& in_reader );
	void destroy( );

	inline u8 get_font_size( ) const { return m_char_height; }

	u8 get_char_width( char const in_char ) const;
	u32 get_string_width( pcstr const in_str ) const;
	u32 get_string_width( pcstr const in_str, uptr const in_length ) const;

	void render_string(
		pcstr const in_caption, uptr const in_length, math::u16x2 const in_position,
		u16 const in_font_size, math::half4 const in_color, render::ui_batch& in_batch ) const;
	
	void render_multiline_string(
		pcstr const in_caption, uptr const in_length, math::u16x2 const in_position, u16 const in_line_interval,
		u16 const in_font_size, math::half4 const in_color, render::ui_batch& in_batch ) const;

private:
	enum { max_char_count = 256 - 32 };

	lib::fixed_array<u8, max_char_count> m_chars_width;
	math::u16x2 m_texture_dimensions;
	render::texture_id m_texture;
	u8 m_chars_in_row;
	u8 m_chars_in_column;
	u8 m_char_height;
	u8 m_start_char;
	u8 m_char_count;

};

} // namespace ui

#endif // #ifndef GUARD_UI_FONT_H_INCLUDED
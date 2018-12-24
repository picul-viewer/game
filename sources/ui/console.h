#ifndef __ui_console_h_included_
#define __ui_console_h_included_

#include <types.h>
#include <lib/extensible_array.h>
#include <lib/linear_allocator.h>
#include <math/vector.h>

#include "font.h"
#include "text_edit_controller.h"

namespace ui {

class console
{
public:
	typedef void( *callback_type )( pcstr );

public:
	void create( pcstr const in_font_path, u8 const in_visible_lines );
	void destroy( );
	
	void on_char( u32 const in_key );
	void on_input( );

	void render( render::ui_batch& in_batch ) const;
	
	void print( pcstr const in_string, uptr const in_length );
	template<typename ... Args>
	inline void printf( pcstr const in_mask, Args const& ... args );

	inline void set_background_color		( math::half4 const& in_value ) { m_background_color		= in_value; }
	inline void set_text_color				( math::half4 const& in_value ) { m_text_color				= in_value; }
	inline void set_edit_background_color	( math::half4 const& in_value ) { m_edit_background_color	= in_value; }
	inline void set_edit_text_color			( math::half4 const& in_value ) { m_edit_text_color			= in_value; }
	inline void set_selection_color			( math::half4 const& in_value ) { m_selection_color			= in_value; }
	inline void set_cursor_color			( math::half4 const& in_value ) { m_cursor_color			= in_value; }

	inline void set_font_size				( u8 const in_value ) { m_font_size				= in_value ? in_value : m_font.get_font_size( ); }

	inline void set_callback				( callback_type const& in_value ) { m_callback = in_value; }

		   void set_corners_position		( math::u16x4 const in_value );
	inline void set_border_size				( u16 const in_value ) { m_border_size	= in_value; }

	inline void set_scroll_speed_in_lines	( u8 const in_value ) { m_scroll_speed_in_lines = in_value; }

	void set_default_colors( );

private:
	void update_line_endings( pcstr const in_new_chars, uptr const in_length );

private:
	enum {
		text_edit_memory_size = 128
	};

private:
	math::half4 m_background_color;
	math::half4 m_text_color;
	math::half4 m_edit_background_color;
	math::half4 m_edit_text_color;
	math::half4 m_selection_color;
	math::half4 m_cursor_color;
	
	text_edit_controller m_text_edit;
	char m_text_edit_memory[text_edit_memory_size];

	dynamic_linear_allocator<Memory_Page_Size, 1024> m_text;
	extensible_array<u32> m_next_line_indices;

	font m_font;

	callback_type m_callback;
	
	math::u16x4 m_background_position;
	math::u16x4 m_edit_background_position;
	math::u16x2 m_text_position;
	math::u16x2 m_edit_text_position;
	u16 m_border_size;
	u16 m_last_line_index;

	u8 m_font_size;
	u8 m_visible_lines_count;
	u8 m_scroll_speed_in_lines;

};

} // namespace ui

#include "console_inline.h"

#endif // #ifndef __ui_console_h_included_
#include "text_edit_controller.h"
#include <Windows.h>
#include <macros.h>
#include <lib/allocator.h>
#include <lib/strings.h>
#include <system/input.h>
#include <system/window_input.h>
#include <system/window_input_state.h>

namespace ui {

void text_edit_controller::create( pstr const in_text_memory, uptr const in_text_memory_size )
{
	ASSERT				( in_text_memory );
	m_data				= in_text_memory;
	
	ASSERT				( in_text_memory_size );
	ASSERT_CMP			( in_text_memory_size, <, 32 * Kb );
	m_capacity			= (u16)in_text_memory_size;

	clear				( );
}

void text_edit_controller::destroy( )
{ }

void text_edit_controller::on_char( u32 const in_key )
{
	if ( ( in_key < 32 ) || ( in_key > 255 ) )
		return;

	if ( m_selection_length == 0 )
	{
		if ( m_length == m_capacity )
			return;

		u16 const tail_length = m_length - m_selection_start;

		if ( tail_length != 0 )
		{
			pstr const temp = stack_allocate( tail_length );
			strings::copy_n( temp, m_data + m_selection_start, tail_length );
			strings::copy_n( m_data + m_selection_start + 1, temp, tail_length );
		}

		m_data[m_selection_start] = (u8)in_key;

		++m_selection_start;
		++m_length;
		
		m_data[m_length] = '\0';
	}
	else
	{
		u16 const begin_offset = m_selection_start;
		u16 const end_offset = m_selection_start + m_selection_length;

		u16 const copy_dest_offset = math::min( begin_offset, end_offset ) + 1;
		u16 const copy_src_offset = math::max( begin_offset, end_offset );

		u16 const tail_length = m_length - copy_src_offset;

		if ( copy_dest_offset != copy_src_offset )
		{
			if ( tail_length != 0 )
				strings::copy_n( m_data + copy_dest_offset, m_data + copy_src_offset, tail_length );

			m_length = copy_dest_offset + tail_length;
			m_data[m_length] = '\0';
		}

		m_data[copy_dest_offset - 1] = (u8)in_key;

		m_selection_start = copy_dest_offset;
		m_selection_length = 0;
	}
}

void text_edit_controller::on_input( )
{
	using namespace sys;

	RAWKEYBOARD const& keyboard = ( (RAWINPUT*)g_window_input.get_raw_input( ) )->data.keyboard;

	bool const key_down = ( keyboard.Flags & 0x1 ) == 0;

	if ( !key_down )
		return;

	key const k = (key)keyboard.VKey;

	switch ( k )
	{
		case key::left:
		{
			bool const is_shift = g_input_state.get_keyboard( ).is_key_pressed( key::shift );

			if ( is_shift )
			{
				m_selection_length -= ( m_selection_start + m_selection_length != 0 ) ? 1 : 0;
			}
			else if ( m_selection_length == 0 )
			{
				m_selection_start -= ( m_selection_start != 0 ) ? 1 : 0;
			}
			else
			{
				m_selection_start += ( m_selection_length < 0 ) ? m_selection_length : 0;
				m_selection_length = 0;
			}

			break;
		}

		case key::right:
		{
			bool const is_shift = g_input_state.get_keyboard( ).is_key_pressed( key::shift );

			if ( is_shift )
			{
				m_selection_length += ( m_selection_start + m_selection_length != m_length ) ? 1 : 0;
			}
			else if ( m_selection_length == 0 )
			{
				m_selection_start += ( m_selection_start != m_length ) ? 1 : 0;
			}
			else
			{
				m_selection_start += ( m_selection_length > 0 ) ? m_selection_length : 0;
				m_selection_length = 0;
			}

			break;
		}
		
		case key::home:
		{
			bool const is_shift = g_input_state.get_keyboard( ).is_key_pressed( key::shift );

			if ( is_shift )
			{
				m_selection_length = -m_selection_start;
			}
			else
			{
				m_selection_start = 0;
				m_selection_length = 0;
			}

			break;
		}

		case key::end:
		{
			bool const is_shift = g_input_state.get_keyboard( ).is_key_pressed( key::shift );

			if ( is_shift )
			{
				m_selection_length = m_length - m_selection_start;
			}
			else
			{
				m_selection_start = m_length;
				m_selection_length = 0;
			}

			break;
		}

		case key::backspace:
		{
			if ( m_selection_length == 0 )
			{
				if ( m_selection_start == 0 )
					break;

				uptr const tail_length = m_length - m_selection_start;

				if ( tail_length != 0 )
					strings::copy_n( m_data + m_selection_start - 1, m_data + m_selection_start, tail_length );

				--m_length;
				--m_selection_start;
			}
			else
			{
				u16 const begin_offset = m_selection_start;
				u16 const end_offset = m_selection_start + m_selection_length;

				u16 const copy_dest_offset = math::min( begin_offset, end_offset );
				u16 const copy_src_offset = math::max( begin_offset, end_offset );

				u16 const tail_length = m_length - copy_src_offset;

				if ( tail_length )
					strings::copy_n( m_data + copy_dest_offset, m_data + copy_src_offset, tail_length );

				m_length -= copy_src_offset - copy_dest_offset;

				m_selection_start = copy_dest_offset;
				m_selection_length = 0;
			}

			m_data[m_length]	= '\0';

			break;
		}

		case key::del:
		{
			if ( m_selection_length == 0 )
			{
				if ( m_selection_start == m_length )
					break;

				uptr const tail_length = m_length - m_selection_start - 1;

				if ( tail_length != 0 )
					strings::copy_n( m_data + m_selection_start, m_data + m_selection_start + 1, tail_length );

				--m_length;
			}
			else
			{
				u16 const begin_offset = m_selection_start;
				u16 const end_offset = m_selection_start + m_selection_length;

				u16 const copy_dest_offset = math::min( begin_offset, end_offset );
				u16 const copy_src_offset = math::max( begin_offset, end_offset );

				u16 const tail_length = m_length - copy_src_offset;

				if ( tail_length )
					strings::copy_n( m_data + copy_dest_offset, m_data + copy_src_offset, tail_length );

				m_length -= copy_src_offset - copy_dest_offset;

				m_selection_start = copy_dest_offset;
				m_selection_length = 0;
			}
			
			m_data[m_length]	= '\0';

			break;
		}
	}
}

pcstr text_edit_controller::get_text( ) const
{
	return m_data;
}

uptr text_edit_controller::get_length( ) const
{
	return m_length;
}

uptr text_edit_controller::get_selection_start( ) const
{
	return m_selection_start;
}

ptr text_edit_controller::get_selection_length( ) const
{
	return m_selection_length;
}

void text_edit_controller::clear( )
{
	m_length			= 0;
	m_selection_start	= 0;
	m_selection_length	= 0;
	m_data[m_length]	= '\0';
}

} // namespace ui
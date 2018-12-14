#include "window_input_state.h"
#include <Windows.h>
#include <macros.h>

namespace sys {

void window_input_state::on_message( pvoid raw_data )
{
    RAWINPUT* const raw = (RAWINPUT*)raw_data;

    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
		key const k			= (key)raw->data.keyboard.VKey;
		bool const pressed	= !( raw->data.keyboard.Flags & 0x1 );

		m_keyboard.set_key_pressed( k, pressed );

		if ( ( k == key::shift ) || ( k == key::control ) || ( k == key::alt ) )
		{
			static_assert( ( (u32)key::l_shift + 1		== (u32)key::r_shift ) &&
						   ( (u32)key::l_control + 1	== (u32)key::r_control ) &&
						   ( (u32)key::l_alt + 1		== (u32)key::r_alt ), "this code needs to be reworked" );

			enum { additive = (u32)key::l_shift - 2 * (u32)key::shift };

			static_assert( ( (u32)key::shift * 2 + additive		== (u32)key::l_shift ) &&
						   ( (u32)key::control * 2 + additive	== (u32)key::l_control ) &&
						   ( (u32)key::alt * 2 + additive		== (u32)key::l_alt ), "this code needs to be reworked" );

			key const actual_key = (key)( (u32)k * 2 + additive + ( ( raw->data.keyboard.Flags & RI_KEY_E0 ) ? 1 : 0 ) );

			m_keyboard.set_key_pressed( actual_key, pressed );
		}
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE)
    {
		if ( ( raw->data.mouse.usFlags & 0x3 ) == MOUSE_MOVE_RELATIVE )
			m_mouse.mouse_position += math::s32x2( raw->data.mouse.lLastX, raw->data.mouse.lLastY );
		else
			m_mouse.mouse_position = math::s32x2( raw->data.mouse.lLastX, raw->data.mouse.lLastY );

		u32 button_flags = raw->data.mouse.usButtonFlags;

		for ( u32 i = 0; i < (u32)mouse_button::count; ++i )
		{
			mouse_button const button = (mouse_button)i;

			if ( button_flags & 0x1 )
				m_mouse.set_button_pressed( button, true );
			
			if ( button_flags & 0x2 )
				m_mouse.set_button_pressed( button, false );

			button_flags >>= 2;
		}

		if ( raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL )
			m_mouse.wheel_delta += raw->data.mouse.usButtonData;
    }
	else
		UNREACHABLE_CODE
}

window_input_state g_input_state;

} // namespace sys
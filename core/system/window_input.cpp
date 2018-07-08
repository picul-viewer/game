#include "window_input.h"
#include <macros.h>
#include <Windows.h>

namespace sys {

void window_input::create( )
{
	enum { device_count = 2 };

	RAWINPUTDEVICE raw_input_device[device_count];

	u32 device_index = 0;

	// Mouse
	raw_input_device[device_index].usUsagePage	= 0x01;
	raw_input_device[device_index].usUsage		= 0x02;
	raw_input_device[device_index].dwFlags		= RIDEV_NOLEGACY;
	raw_input_device[device_index].hwndTarget	= 0;
	++device_index;
	
	// Keyboard
	raw_input_device[device_index].usUsagePage	= 0x01; 
	raw_input_device[device_index].usUsage		= 0x06; 
	raw_input_device[device_index].dwFlags		= RIDEV_NOLEGACY;
	raw_input_device[device_index].hwndTarget	= 0;
	++device_index;

	ASSERT( device_index == device_count );

	auto result = RegisterRawInputDevices( raw_input_device, 2, sizeof(RAWINPUTDEVICE) );

	ASSERT( result != FALSE );
}

void window_input::destroy( )
{
	//
}

void window_input::on_message( pvoid handle )
{
	u32 size;

    GetRawInputData( (HRAWINPUT)handle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER) );

	ASSERT( size <= 128 * Kb );
	pvoid data = alloca( size );

    u32 read_size = GetRawInputData( (HRAWINPUT)handle, RID_INPUT, data, &size, sizeof(RAWINPUTHEADER) );
	ASSERT( read_size == size );

    RAWINPUT* raw = (RAWINPUT*)data;

	// Update is non-atomic for now
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
		else
			m_keyboard.set_key_pressed_interlocked( k, pressed );
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
}

window_input g_window_input;

} // namespace sys
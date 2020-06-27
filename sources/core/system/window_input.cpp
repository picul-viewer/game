#include "window_input.h"
#include <macros.h>
#include <Windows.h>
#include "window_input_state.h"

void sys::window_input::create( )
{
	static_assert( raw_input_size == sizeof(RAWINPUT), "internal input store has incorrect size" );

	enum { device_count = 2 };

	RAWINPUTDEVICE raw_input_device[device_count];

	u32 device_index = 0;

	// Mouse
	raw_input_device[device_index].usUsagePage	= 0x01;
	raw_input_device[device_index].usUsage		= 0x02;
	raw_input_device[device_index].dwFlags		= 0;
	raw_input_device[device_index].hwndTarget	= 0;
	++device_index;
	
	// Keyboard
	raw_input_device[device_index].usUsagePage	= 0x01; 
	raw_input_device[device_index].usUsage		= 0x06; 
	raw_input_device[device_index].dwFlags		= 0;
	raw_input_device[device_index].hwndTarget	= 0;
	++device_index;

	ASSERT_CMP( device_index, ==, device_count );

	BOOL const result = RegisterRawInputDevices( raw_input_device, 2, sizeof(RAWINPUTDEVICE) );

	ASSERT( result != FALSE );
}

void sys::window_input::destroy( )
{
	//
}

void sys::window_input::on_message( pvoid handle )
{
	// NOTE: don't support joistics for now, so sizeof(RAWINPUT) is known.
	u32 size = sizeof(RAWINPUT);
	u32 read_size = GetRawInputData( (HRAWINPUT)handle, RID_INPUT, m_last_input, &size, sizeof(RAWINPUTHEADER) );
	ASSERT_CMP( read_size, <=, sizeof(RAWINPUT) );

	g_input_state.on_message( m_last_input );
}

namespace sys {
window_input g_window_input;
} // namespace sys
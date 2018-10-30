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
	raw_input_device[device_index].dwFlags		= 0;
	raw_input_device[device_index].hwndTarget	= 0;
	++device_index;
	
	// Keyboard
	raw_input_device[device_index].usUsagePage	= 0x01; 
	raw_input_device[device_index].usUsage		= 0x06; 
	raw_input_device[device_index].dwFlags		= 0;
	raw_input_device[device_index].hwndTarget	= 0;
	++device_index;

	ASSERT( device_index == device_count );

	BOOL const result = RegisterRawInputDevices( raw_input_device, 2, sizeof(RAWINPUTDEVICE) );

	ASSERT( result != FALSE );
}

void window_input::destroy( )
{
	//
}

void window_input::get_input_data( pvoid handle, pvoid output_data )
{
	// NOTE: don't support joistics for now, so sizeof(RAWINPUT) is known.
	u32 size = sizeof(RAWINPUT);
    u32 read_size = GetRawInputData( (HRAWINPUT)handle, RID_INPUT, output_data, &size, sizeof(RAWINPUTHEADER) );
	ASSERT( read_size <= sizeof(RAWINPUT) );
}

window_input g_window_input;

} // namespace sys
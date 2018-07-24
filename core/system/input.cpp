#include "input.h"

#include <macros.h>
#include <threads/interlocked.h>

namespace sys {

void mouse::set_button_pressed( mouse_button button, bool pressed )
{
	if ( pressed )
		button_data |= ( 1 << (u32)button );
	else
		button_data &= ~( 1 << (u32)button );
}

bool mouse::is_button_pressed( mouse_button button ) const
{
	return ( button_data & ( 1 << (u32)button ) ) != 0;
}


void keyboard::set_key_pressed( key k, bool pressed )
{
	ASSERT( (u32)k <= 0xFF );
	u32 index = key_to_keyboard_index[(u32)k];

	if ( pressed )
		data.data[index / 32] |= ( 1 << ( index % 32 ) );
	else
		data.data[index / 32] &= ~( 1 << ( index % 32 ) );
}

void keyboard::set_key_pressed_interlocked( key k, bool pressed )
{
	ASSERT( (u32)k <= 0xFF );
	u32 index = key_to_keyboard_index[(u32)k];

	if ( pressed )
		interlocked_or( static_cast<mt_u32&>( data.data[index / 32] ), ( 1 << ( index % 32 ) ) );
	else
		interlocked_and( static_cast<mt_u32&>( data.data[index / 32] ), ~( 1 << ( index % 32 ) ) );
}

bool keyboard::is_key_pressed( key k ) const
{
	ASSERT( (u32)k <= 0xFF );
	u32 index = key_to_keyboard_index[(u32)k];
	return ( data.data[index / 32] & ( 1 << ( index % 32 ) ) ) != 0;
}

} // namespace sys
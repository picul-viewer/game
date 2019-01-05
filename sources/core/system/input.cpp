#include "input.h"

#include <macros.h>
#include "interlocked.h"

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
	u32 const index = (u32)k;
	ASSERT_CMP( index, <=, 0xFFu );

	if ( pressed )
		data[index / 32] |= ( 1 << ( index % 32 ) );
	else
		data[index / 32] &= ~( 1 << ( index % 32 ) );
}

bool keyboard::is_key_pressed( key k ) const
{
	u32 const index = (u32)k;
	ASSERT_CMP( index, <=, 0xFFu );

	return ( data[index / 32] & ( 1 << ( index % 32 ) ) ) != 0;
}

} // namespace sys
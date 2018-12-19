#ifndef __core_input_h_included_
#define __core_input_h_included_

#include <types.h>
#include <math/vector.h>

namespace sys {

enum class mouse_button
{
	left = 0,
	rigth,
	middle,

	count
};

mem_align(16)
struct mouse
{
	math::s32x2 mouse_position;
	u32 button_data;
	s32 wheel_delta;
	
	void set_button_pressed( mouse_button button, bool pressed );

	bool is_button_pressed( mouse_button button ) const;
};

enum class key {
	backspace			= 0x08,
	tab					= 0x09,

	clear				= 0x0C,
	enter				= 0x0D,

	shift				= 0x10,
	control				= 0x11,
	alt					= 0x12,

	pause				= 0x13,

	escape				= 0x1B,

	space				= 0x20,
	page_up				= 0x21,
	page_down			= 0x22,
	end					= 0x23,
	home				= 0x24,

	left				= 0x25,
	up					= 0x26,
	right				= 0x27,
	down				= 0x28,

	snapshot			= 0x2C,

	insert				= 0x2D,
	del					= 0x2E,

	number0				= 0x30,
	number1				= 0x31,
	number2				= 0x32,
	number3				= 0x33,
	number4				= 0x34,
	number5				= 0x35,
	number6				= 0x36,
	number7				= 0x37,
	number8				= 0x38,
	number9				= 0x39,

	letter_a			= 0x41,
	letter_b			= 0x42,
	letter_c			= 0x43,
	letter_d			= 0x44,
	letter_e			= 0x45,
	letter_f			= 0x46,
	letter_g			= 0x47,
	letter_h			= 0x48,
	letter_i			= 0x49,
	letter_j			= 0x4A,
	letter_k			= 0x4B,
	letter_l			= 0x4C,
	letter_m			= 0x4D,
	letter_n			= 0x4E,
	letter_o			= 0x4F,
	letter_p			= 0x50,
	letter_q			= 0x51,
	letter_r			= 0x52,
	letter_s			= 0x53,
	letter_t			= 0x54,
	letter_u			= 0x55,
	letter_v			= 0x56,
	letter_w			= 0x57,
	letter_x			= 0x58,
	letter_y			= 0x59,
	letter_z			= 0x5A,

	numpad0				= 0x60,
	numpad1				= 0x61,
	numpad2				= 0x62,
	numpad3				= 0x63,
	numpad4				= 0x64,
	numpad5				= 0x65,
	numpad6				= 0x66,
	numpad7				= 0x67,
	numpad8				= 0x68,
	numpad9				= 0x69,

	multiply			= 0x6A,
	add					= 0x6B,
	separator			= 0x6C,
	substract			= 0x6D,
	decimal				= 0x6E,
	divide				= 0x6F,

	f1					= 0x70,
	f2					= 0x71,
	f3					= 0x72,
	f4					= 0x73,
	f5					= 0x74,
	f6					= 0x75,
	f7					= 0x76,
	f8					= 0x77,
	f9					= 0x78,
	f10					= 0x79,
	f11					= 0x7A,
	f12					= 0x7B,
	f13					= 0x7C,
	f14					= 0x7D,
	f15					= 0x7E,
	f16					= 0x7F,
	f17					= 0x80,
	f18					= 0x81,
	f19					= 0x82,
	f20					= 0x83,
	f21					= 0x84,
	f22					= 0x85,
	f23					= 0x86,
	f24					= 0x87,

	numlock				= 0x90,
	scroll				= 0x91,

	l_shift				= 0xA0,
	r_shift				= 0xA1,
	l_control			= 0xA2,
	r_control			= 0xA3,
	l_alt				= 0xA4,
	r_alt				= 0xA5,

	volume_mute			= 0xAD,
	volume_down			= 0xAE,
	volume_up			= 0xAF,
	media_next_track	= 0xB0,
	media_prev_track	= 0xB1,
	media_stop			= 0xB2,
	media_play_pause	= 0xB3
};

mem_align(16)
struct keyboard
{
	u32 data[8];

	void set_key_pressed( key k, bool pressed );
	
	bool is_key_pressed( key k ) const;
};

} // namespace sys

#endif // #ifndef __core_input_h_included_
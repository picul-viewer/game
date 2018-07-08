#ifndef __core_input_h_included_
#define __core_input_h_included_

#include <types.h>
#include <macros.h>
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
	
	bool set_button_pressed( mouse_button button, bool pressed );

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

static const char key_to_keyboard_index[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x02, 0x03, 0xff, 0xff,
	0x04, 0x05, 0x06, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0xff, 0xff, 0xff, 0xff,
	0x09, 0xff, 0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0x10, 0x11, 0x12, 0xff,
	0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
	0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
	0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
	0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x5f, 0x60, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x67, 0x68, 0x69,
	0x6a, 0x6b, 0x6c, 0x6d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static const char keyboard_index_to_key[] = {
	0x08, 0x09,
	0x0C, 0x0D,
	0x10, 0x11, 0x12,
	0x13,
	0x1B,
	0x20,
	0x23, 0x24,
	0x25, 0x26, 0x27, 0x28,
	0x2C,
	0x2D, 0x2E,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
	0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B,
	0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x90, 0x91,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5,
	0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3
};

mem_align(16)
struct keyboard
{
	math::u32x4 data;

	void set_key_pressed( key k, bool pressed );
	void set_key_pressed_interlocked( key k, bool pressed );
	
	bool is_key_pressed( key k ) const;
};

} // namespace sys

#endif // #ifndef __core_input_h_included_
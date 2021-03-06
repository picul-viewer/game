#ifndef GUARD_CORE_WINDOW_INPUT_STATE_H_INCLUDED
#define GUARD_CORE_WINDOW_INPUT_STATE_H_INCLUDED

#include <types.h>
#include "input.h"

namespace sys {

class window_input_state
{
public:
	inline mouse const& get_mouse( ) const { return m_mouse; }
	inline keyboard const& get_keyboard( ) const { return m_keyboard; }

private:
	friend class window_input;
	void on_message( pvoid raw_data );

private:
	mouse		m_mouse;
	keyboard	m_keyboard;

};

extern window_input_state g_input_state;

} // namespace sys

#endif // #ifndef GUARD_CORE_WINDOW_INPUT_STATE_H_INCLUDED
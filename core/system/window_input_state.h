#ifndef __core_window_input_state_h_included_
#define __core_window_input_state_h_included_

#include <types.h>
#include "input.h"

namespace sys {

class window_input_state
{
public:	
	void on_message( pvoid raw_data );

	inline mouse const& get_mouse( ) const { return m_mouse; }
	inline keyboard const& get_keyboard( ) const { return m_keyboard; }

protected:
	mouse		m_mouse;
	keyboard	m_keyboard;

};

} // namespace sys

#endif // #ifndef __core_window_input_state_h_included_
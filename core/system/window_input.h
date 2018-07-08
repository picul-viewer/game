#ifndef __core_window_input_h_included_
#define __core_window_input_h_included_

#include <types.h>
#include <math/vector.h>
#include "input.h"

namespace sys {

class window_input
{
public:
	void create( );
	void destroy( );

	void on_message( pvoid handle );

	inline mouse const& get_mouse( ) const { return m_mouse; }
	inline keyboard const& get_keyboard( ) const { return m_keyboard; }

protected:
	mouse		m_mouse;
	keyboard	m_keyboard;

};

extern window_input g_window_input;

} // namespace sys

#endif // #ifndef __core_window_input_h_included_
#ifndef GUARD_CORE_WINDOW_INPUT_H_INCLUDED
#define GUARD_CORE_WINDOW_INPUT_H_INCLUDED

#include <types.h>

namespace sys {

class window_input
{
public:
	void create( );
	void destroy( );

	void on_message( pvoid handle );

	inline pointer get_raw_input( ) { return m_last_input; }

private:
	enum { raw_input_size = 48 };

	u8 m_last_input[raw_input_size];

};

extern window_input g_window_input;

} // namespace sys

#endif // #ifndef GUARD_CORE_WINDOW_INPUT_H_INCLUDED
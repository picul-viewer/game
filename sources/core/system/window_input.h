#ifndef __core_window_input_h_included_
#define __core_window_input_h_included_

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

#endif // #ifndef __core_window_input_h_included_
#ifndef __core_window_input_h_included_
#define __core_window_input_h_included_

#include <types.h>

namespace sys {

class window_input
{
public:
	void create( );
	void destroy( );

	void get_input_data( pvoid handle, pvoid output_data );
};

extern window_input g_window_input;

} // namespace sys

#endif // #ifndef __core_window_input_h_included_
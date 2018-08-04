#ifndef __core_window_h_included_
#define __core_window_h_included_

#include <types.h>
#include <math/vector.h>

namespace sys {

class window
{
public:
	window( ) = default;
	~window( ) = default;

	void create( pcstr title, math::s32x2 const& dimensions, bool fullscreen, pointer window_procedure );
	void destroy( );
	void run( );
	void exit( );

	inline pvoid get_hwnd( ) const { return m_hwnd; }

protected:
	pvoid m_hwnd;
};

} // namespace sys

#endif // #ifndef __core_window_h_included_
#ifndef GUARD_CORE_WINDOW_H_INCLUDED
#define GUARD_CORE_WINDOW_H_INCLUDED

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

#endif // #ifndef GUARD_CORE_WINDOW_H_INCLUDED
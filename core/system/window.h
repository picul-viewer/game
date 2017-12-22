#ifndef __core_window_h_included_
#define __core_window_h_included_

#include <core/types.h>
#include <core/std.h>
#include <core/math.h>
#include <Windows.h>

struct window_init
{
	pcstr title;
	math::s16x2 dimensions;
	bool fullscreen;
	bool visible;
};

template<typename Events>
class window
{
public:
	window( ) = default;
	~window( ) = default;

	void create( window_init const& init );
	void destroy( );
	void show( int param );
	bool is_opened( );
	void run( );

	inline HWND get_hwnd( ) const { return m_hwnd; }

protected:
	HWND		m_hwnd;

	static LRESULT CALLBACK wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
};

struct window_default_events
{
	static void create( HWND hwnd ) { }
	static void destroy( ) { }
	static void activate( bool active ) { }
	static void resize( math::u16x2 new_dimensions ) { }
	static void update( ) { }
};

#include "window_inline.h"

#endif // #ifndef __core_window_h_included_
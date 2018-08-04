#include "window.h"

#include <Windows.h>
#include <windowsx.h>
#include <macros.h>

namespace sys {

void window::create( pcstr title, math::s32x2 const& dimensions, bool fullscreen, pointer window_procedure )
{
	ASSERT				( dimensions > 0 );

	HINSTANCE inst		= GetModuleHandle( nullptr );

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= window_procedure;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= inst;
	wcex.hbrBackground	= (HBRUSH)( COLOR_WINDOW + 1 );
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= title;
	wcex.hIcon			= LoadIcon	( inst, IDI_APPLICATION );
	wcex.hCursor		= LoadCursor( nullptr, IDC_ARROW );
	wcex.hIconSm		= LoadIcon	( wcex.hInstance, IDI_APPLICATION );

	auto result			= RegisterClassEx( &wcex );
	ASSERT				( result != 0 );

	RECT rect			= { 0, 0, dimensions.x, dimensions.y };

	DWORD style			= ( fullscreen ? ( WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ) : WS_OVERLAPPEDWINDOW ) | WS_VISIBLE;

	AdjustWindowRect	( &rect, style, FALSE );
	
	s32 width			= rect.right - rect.left;
	s32 height			= rect.bottom - rect.top;

	s32 x				= ( GetSystemMetrics( SM_CXSCREEN ) - width ) / 2;
	s32 y				= ( GetSystemMetrics( SM_CYSCREEN ) - height ) / 2;

	m_hwnd				= CreateWindow( title, title, style, x, y, width, height, nullptr, nullptr, inst, this );

	ASSERT				( m_hwnd );

	ShowWindow			( (HWND)m_hwnd, SW_SHOW );
}

void window::destroy( )
{
	ASSERT				( m_hwnd );

	enum { class_name_max_length = 256 };
	char class_name[class_name_max_length];
	RealGetWindowClass	( (HWND)m_hwnd, class_name, class_name_max_length );

	DestroyWindow		( (HWND)m_hwnd );
	m_hwnd				= nullptr;
	
	HINSTANCE inst		= GetModuleHandle( nullptr );
	UnregisterClass		( class_name, inst );
}

void window::run( )
{
	MSG msg;

	do
	{
		if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage	( &msg );
		}
	}
	while ( msg.message != WM_QUIT );
}

void window::exit( )
{
	SendMessage( (HWND)m_hwnd, WM_CLOSE, 0, 0 );
}

} // namespace sys
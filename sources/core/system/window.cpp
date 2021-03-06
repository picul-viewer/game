#include "window.h"

#include <Windows.h>
#include <windowsx.h>
#include <macros.h>

void sys::window::create( pcstr title, math::s32x2 const& dimensions, bool fullscreen, pointer window_procedure )
{
	ASSERT_CMP			( dimensions.x, >, 0 );
	ASSERT_CMP			( dimensions.y, >, 0 );

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

	ATOM const result	= RegisterClassEx( &wcex );
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

void sys::window::destroy( )
{
	ASSERT				( m_hwnd );

	DestroyWindow		( (HWND)m_hwnd );
	m_hwnd				= nullptr;
}

void sys::window::run( )
{
	MSG msg;

	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage	( &msg );
	}
}

void sys::window::exit( )
{
	SendMessage( (HWND)m_hwnd, WM_CLOSE, 0, 0 );
}
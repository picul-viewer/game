#ifndef __core_window_inline_h_included_
#define __core_window_inline_h_included_

#include <windowsx.h>

template<typename Events>
LRESULT CALLBACK window<Events>::wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	window *w = nullptr;
	if ( message == WM_NCCREATE )
	{
		CREATESTRUCT *createStruct = (CREATESTRUCT*)lParam;
		w = (window*)createStruct->lpCreateParams;
		SetWindowLongPtr( hWnd, GWLP_USERDATA, LONG_PTR( w ) );
	}
	else
	{
		w = (window*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
	}

	switch ( message )
	{
	case WM_CREATE:
		Events::create	( hWnd );
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint		( w->m_hwnd, &ps );
		EndPaint		( w->m_hwnd, &ps );
		break;

	case WM_DESTROY:
		Events::destroy	( );
		PostQuitMessage	( 0 );
		break;

	case WM_SIZE:
		Events::resize	( math::u16x2( LOWORD( lParam ), HIWORD( lParam ) ) );
		break;

	case WM_ACTIVATE:
		Events::activate( wParam != 0 );
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
		break;
	}

	return 0;
}

template<typename Events>
void window<Events>::create( window_init const& init )
{
	struct window_class
	{
		window_class( )
		{
			HINSTANCE inst		= GetModuleHandle( nullptr );

			WNDCLASSEX wcex;
			wcex.cbSize			= sizeof(WNDCLASSEX);
			wcex.style			= CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc	= &window<Events>::wndProc;
			wcex.cbClsExtra		= 0;
			wcex.cbWndExtra		= 0;
			wcex.hInstance		= inst;
			wcex.hbrBackground	= (HBRUSH)( COLOR_WINDOW + 1 );
			wcex.lpszMenuName	= nullptr;
			wcex.lpszClassName	= get_name( );
			wcex.hIcon			= LoadIcon	( inst, IDI_APPLICATION );
			wcex.hCursor		= LoadCursor( nullptr, IDC_ARROW );
			wcex.hIconSm		= LoadIcon	( wcex.hInstance, IDI_APPLICATION );

			if ( !RegisterClassEx( &wcex ) ) return;
		}

		~window_class( )
		{
			HINSTANCE inst	= GetModuleHandle( nullptr );
			UnregisterClass	( get_name( ), inst );
		}

		char const* get_name( )
		{
			return typeid(window_class).name( );
		}
	};

	static window_class this_class;

	RECT rect		= { 0, 0, init.dimensions.x, init.dimensions.y };

	DWORD style		= init.fullscreen ? WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN : WS_OVERLAPPEDWINDOW;
	if ( init.visible )
		style		|= WS_VISIBLE;

	AdjustWindowRect( &rect, style, FALSE );
	
	int x_pos		= GetSystemMetrics( SM_CXSCREEN );
	int y_pos		= GetSystemMetrics( SM_CYSCREEN );

	if ( init.fullscreen )
	{
		// TODO: this is not working

		DEVMODE mode = { };

		mode.dmSize				= sizeof(DEVMODE);
		mode.dmPelsWidth		= x_pos;
		mode.dmPelsHeight		= y_pos;
		mode.dmFields			= DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings	( &mode, 0 );
	}

	x_pos			= ( x_pos - init.dimensions.x ) / 2;
	y_pos			= ( y_pos - init.dimensions.y ) / 2;

	HINSTANCE inst	= GetModuleHandle( nullptr );
	m_hwnd			= CreateWindow( this_class.get_name( ), init.title, style,
		x_pos, y_pos, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, inst, this );
}

template<typename Events>
void window<Events>::destroy( )
{
	if ( m_hwnd )
	{
		DestroyWindow			( m_hwnd );
		m_hwnd					= nullptr;
		ChangeDisplaySettings	( nullptr, 0 );
	}
}

template<typename Events>
void window<Events>::show( int param )
{
	ShowWindow( m_hwnd, param );
}

template<typename Events>
bool window<Events>::is_opened( )
{
	return IsWindow( m_hwnd ) != FALSE;
}

template<typename Events>
void window<Events>::run( )
{
	MSG msg = { };

	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage	( &msg );
		}
		else
			Events::update	( );
	}
}

#endif // #ifndef __core_window_inline_h_included_
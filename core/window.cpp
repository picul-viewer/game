#include "window.h"
#include <windowsx.h>

LRESULT CALLBACK game_window::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	game_window *window = nullptr;
	if (message == WM_NCCREATE)
	{
		CREATESTRUCT *createStruct = (CREATESTRUCT*)lParam;
		window = (game_window*)createStruct->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(window));
	}
	else
	{
		window = (game_window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	switch (message)
	{
	case WM_CREATE:
		window->hwnd = hWnd;
		window->onCreate();
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window->hwnd, &ps);
		EndPaint(window->hwnd, &ps);
		break;

	case WM_DESTROY:
		window->onDestroy();
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		window->onResize({ LOWORD(lParam), HIWORD(lParam) });
		window->dims = uint16x2(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ACTIVATE:
		window->active = wParam != 0;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

game_window::game_window() {}

game_window::~game_window()
{
	destroy();
}

game_window::game_window(game_window&& r)
{
	hwnd = r.hwnd;
	dims = r.dims;
	active = r.active;
	fullscreen = r.fullscreen;
	r.hwnd = nullptr;
}

game_window& game_window::operator=(game_window&& r)
{
	destroy();
	hwnd = r.hwnd;
	dims = r.dims;
	active = r.active;
	fullscreen = r.fullscreen;
	r.hwnd = nullptr;
	return *this;
}

void game_window::create(const game_windowInit& params)
{
	HINSTANCE hInst = GetModuleHandle(nullptr);

	static bool noClass = true;

	if (noClass)
	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = wndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = "game_window";
		wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex)) return;

		noClass = false;
	}

	dims.x = params.width;
	dims.y = params.height;
	active = false;
	fullscreen = params.fullscreen;

	RECT rc = { 0, 0, params.width, params.height };
	AdjustWindowRect(&rc, params.style, FALSE);
	CreateWindow("game_window", params.title, params.style,		/* see WM_CREATE processing */
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, this);
}

void game_window::destroy()
{
	if (hwnd)
	{
		DestroyWindow(hwnd);
		hwnd = nullptr;
	}
}

void game_window::show(int param)
{
	ShowWindow(hwnd, param);
}

bool game_window::opened()
{
	return IsWindow(hwnd) != FALSE;
}

void game_window::run()
{
	MSG msg{};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (active)
			onUpdate();
	}
}
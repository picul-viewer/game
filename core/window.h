#ifndef __window_h_included_
#define __window_h_included_

#include <Windows.h>
#include "math.h"

struct window_init_struct
{
	const TCHAR *title;
	s16 width, height;
	DWORD style;
	bool fullscreen;
};

class game_window
{
protected:
	HWND hwnd;
	s16x2 dims;
	bool active, fullscreen;

	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void onCreate() {}
	virtual void onDestroy() {}
	virtual void onResize(s16x2 dims) {}
	virtual void onUpdate() {}
public:
	game_window();
	~game_window();

	game_window(const game_window&) = delete;
	game_window& operator=(const game_window&) = delete;
	game_window(game_window&& r);
	game_window& operator=(game_window&& r);

	void create(const window_init_struct& params);
	void destroy();
	void show(int param);
	bool opened();
	void run();

	inline HWND getHwnd() { return hwnd; }
	inline s16x2 getDims() { return dims; }
	inline bool isActive() { return active; }
	inline bool isFullScreen() { return fullscreen; }
};

#endif
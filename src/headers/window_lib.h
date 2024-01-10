#pragma once

#include "platform.h"

// Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// Windows Globals
static HWND window;

// Implementations
LRESULT CALLBACK windows_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (msg)
	{
        case WM_CLOSE:
        {
            running = false;
            break;
        }

        default:
        {
            result = DefWindowProcA(window, msg, wParam, lParam);
            break;
        }
	}

	return result;
}

bool plataform_create_window(int w, int h, const char *title)
{
	HINSTANCE instance = GetModuleHandleA(0);

	WNDCLASSA ws = {};
	ws.hInstance = instance;
	ws.hIcon = LoadIcon(instance, IDI_APPLICATION);
	ws.hCursor = LoadCursor(NULL, IDC_ARROW);
	ws.lpszClassName = title;
	ws.lpfnWndProc = windows_window_callback;

	if (!RegisterClassA(&ws))
	{
		return false;
	}

	int dwStyle = WS_OVERLAPPEDWINDOW;
	window = CreateWindowExA(0, title, title, dwStyle, 100, 100, w, h, NULL, NULL, instance, NULL);

	if (window == NULL)
	{
		return false;
	}

	ShowWindow(window, SW_SHOW);

	return true;
}

void plataform_update_window()
{
	MSG msg;

	while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}
#endif
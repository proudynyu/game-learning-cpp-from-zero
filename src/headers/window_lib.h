#pragma once

#include "platform.h"
#include "engine_lib.h"

// Windows
#define APIENTRY
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
// #define NOMINMAX
#include <windows.h>
#include <glcorearb.h>
#include "wglext.h"

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

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

	// // Creating the fake window to initialize the OpenGL
	{
		window = CreateWindowExA(0, title, title, dwStyle, 100, 100, w, h, NULL, NULL, instance, NULL);

		if (window == NULL)
		{
			SM_ASSERT(false, "Failed to create the fake window");
			return false;
		}

		HDC fakeDc = GetDC(window);

		if (!fakeDc)
		{
			SM_ASSERT(false, "Failed to get the fake window DC");
			return false;
		}

		PIXELFORMATDESCRIPTOR pfd = {0};

		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAlphaBits = 8;
		pfd.cDepthBits = 24;

		int pixelFormat = ChoosePixelFormat(fakeDc, &pfd);

		if (!pixelFormat)
		{
			SM_ASSERT(false, "Failed to choose the pixel format");
			return false;
		}

		if (!SetPixelFormat(fakeDc, pixelFormat, &pfd))
		{
			SM_ASSERT(false, "Failed to set the pixel format");
			return false;
		}

		HGLRC fakeRc = wglCreateContext(fakeDc);

		if (!fakeRc)
		{
			SM_ASSERT(false, "Failed to create the fake OpenGL context");
			return false;
		}

		if (!wglMakeCurrent(fakeDc, fakeRc))
		{
			SM_ASSERT(false, "Failed to make the fake OpenGL context current");
			return false;
		}

		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)
		{
			SM_ASSERT(false, "Failed to load the wglChoosePixelFormatARB or wglCreateContextAttribsARB");
			return false;
		}

		wglMakeCurrent(fakeDc, 0);
		wglDeleteContext(fakeRc);
		ReleaseDC(window, fakeDc);

		DestroyWindow(window);
	}

	// // Actual OpenGL initialization
	{
		// Add in the border size to the window size
		{
			RECT borderRect = {};
			AdjustWindowRectEx(&borderRect, dwStyle, 0, 0);

			w += borderRect.right - borderRect.left;
			h += borderRect.bottom - borderRect.top;
		}

		window = CreateWindowExA(0, title, title, dwStyle, 100, 100, w, h, NULL, NULL, instance, NULL);

		if (window == NULL)
		{
			SM_ASSERT(false, "Failed to create the fake window");
			return false; }

		HDC fakeDc = GetDC(window);

		if (!fakeDc)
		{
			SM_ASSERT(false, "Failed to get the fake window DC");
			return false;
		}

		const int pixelAttribs[] = 
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			0,
		};

		UINT numPixelFormats;
		int pixelFormat = 0;

		if (!wglChoosePixelFormatARB(fakeDc, pixelAttribs, NULL, 1, &pixelFormat, &numPixelFormats))
		{
			SM_ASSERT(false, "Failed to choose the pixel format");
			return false;
		}

		PIXELFORMATDESCRIPTOR pfd = {0};
		DescribePixelFormat(fakeDc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

		if (!SetPixelFormat(fakeDc, pixelFormat, &pfd)) {
			SM_ASSERT(false, "Failed to set the pixel format");
			return false;
		}

		const int contextAttribs[] = 
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0,
		};

		HGLRC fakeRc = wglCreateContextAttribsARB(fakeDc, 0, contextAttribs);
		if(!fakeRc)
		{
			SM_ASSERT(false, "Failed to create the fake OpenGL context");
			return false;
		}

		if (!wglMakeCurrent(fakeDc, fakeRc))
		{
			SM_ASSERT(false, "Failed to make the fake OpenGL context current");
			return false;
		}
	}

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

void *platform_load_gl_function(char *funcName)
{
	PROC proc = wglGetProcAddress(funcName);

	if (!proc)
	{
		static HMODULE module = LoadLibraryA("opengl32.dll");
		proc = GetProcAddress(module, funcName);

		if (!proc)
		{
			SM_ASSERT(false, "Failed to load glCreateProgram");
			return nullptr;
		}
	}

	return (void *)proc;
}
#endif
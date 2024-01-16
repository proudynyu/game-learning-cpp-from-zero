#include "headers/platform.h"

// Windows
#ifdef _WIN32
#include "headers/window_lib.h"
#endif

#include <glcorearb.h>
#include "headers/gl_renderer.h"

int main()
{
	// Create a window based on the platform
	plataform_create_window(1200, 700, "Game Dev");

	while (running)
	{
		// update the window based on the platform
		plataform_update_window();
	}

	return 0;
}
#include "headers/platform.h"

// Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "headers/window_lib.h"
#endif

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
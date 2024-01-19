#include "headers/input.h"
#include "headers/platform.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include <glcorearb.h>

// Windows
#ifdef _WIN32
#include "headers/window_lib.h"
#endif

#include "gl_renderer.cpp"

int main()
{
	BumpAllocator transientStorage = make_bump_allocator(MB(50));
	// Create a window based on the platform
	plataform_create_window(1200, 700, "Game Dev");
	input.screenSizeX = 1200;
	input.screenSizeY = 700;

	bool init = gl_init(&transientStorage);

	if (!init)
	{
		SM_ASSERT(false, "Failed to initialize OpenGL");
		return 1;
	}

	while (running)
	{
		// update the window based on the platform
		plataform_update_window();
		gl_render();

		plataform_swap_buffers();
	}

	return 0;
}
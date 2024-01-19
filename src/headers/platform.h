#pragma once

// Constants
static bool running = true;

// Functions
bool plataform_create_window(int w, int h, const char* title);
void plataform_update_window();
void* platform_load_gl_function(char* funcName);
void plataform_swap_buffers();
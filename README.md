# Celeste Clone

Following the series from [Cakez](https://www.youtube.com/@Cakez77) for creating a clone of Celeste Game using C++.

## Notes

* Will use a bash script to build the project
* Using the compiler g++ for this right now
* We create implementations for Windows and Linux, and a Engine that will be geral
    * Plataform
        * defines the basics name functions (implementation will be in each plataform header)
            * windows_window_callback -> callback for the window process
            * plataform_create_window -> function to create a window
            * plataform_update_window -> function to update the window
    * Engine
        * will define the implementations that will be used for both windows and linux
            * Logger
            * Asserts
            * Memory Alloc
    * Windows
        * will use the windows api from the header `windows.h`
            * Using the *ClassA* specifics

* To simulate what GLFW and GLAD does for us, we need to download the headers from [Khronos](https://registry.khronos.org/OpenGL/index_gl.php)
* this headers will help us define the functions pointers to use OpenGL (GLAD does this behind the abstraction)


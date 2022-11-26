/*************************************************************************
				[PUBLIC DOMAIN]

				IO_API
Read IO_API.h for details about this software.

*************************************************************************/

//CHANGE INCLUDE HERE:
#ifdef APP_TYPE_3D

#ifdef WIN32
//#include "ExampleImplimentations/SDL2/IO_API_SDL2_GL.h"
//#include "ExampleImplimentations/Magnum/IO_API_Magnum.h"
#include "ExampleImplimentations/GLFW/IO_API_GLFW_GL.h"
#define STB_IMAGE_IMPLEMENTATION
extern "C" {
	#include <stb/stb_image.h>
	#include <depsVisible/glew/src/glew.c>
}
#else
//#include "ExampleImplimentations/Magnum/IO_API_Magnum.h"
#error "Windows only, other platforms do not have 3D support yet!"
#endif

#else
#ifdef WIN32
#include "ExampleImplimentations/Windows/IO_API_WIN32.h"
#else
//#include "ExampleImplimentations/SDL2/IO_API_SDL2.h"
#error "Windows only, other platforms don't have 3D support yet!'"
#endif
#endif
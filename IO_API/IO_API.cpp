/*************************************************************************
				[PUBLIC DOMAIN]

				IO_API
Read IO_API.h for details about this software.

*************************************************************************/

//CHANGE INCLUDE HERE:
#ifdef APP_TYPE_3D
#include "ExampleImplimentations/Magnum/IO_API_Magnum.h"
#else
#include "ExampleImplimentations/SDL2/IO_API_SDL2.h"
#endif
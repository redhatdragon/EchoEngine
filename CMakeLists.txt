cmake_minimum_required(VERSION 3.19)
project(EchoEngine)

add_library(EchoEngine STATIC
	Asset.cpp
	Widget.cpp
	Name.cpp
	profiler.cpp
	deps/BitBool/BitBool.cpp
	IO_API/IO_API.c
	IO_API/GUI/GUI.c
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_image)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_ttf)
target_include_directories(EchoEngine PRIVATE
	#../include
	#include
	deps
	deps/SDL2/include
	${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(EchoEngine
	SDL2
	SDL2_image
	SDL2_ttf
	SDL2main
	#SDL2-static
)
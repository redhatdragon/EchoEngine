cmake_minimum_required(VERSION 3.19)
project(TouhouWarriors)


add_executable(TouhouWarriors
	main.cpp
)
target_include_directories(TouhouWarriors PRIVATE
	../include
	include
)
target_link_directories(TouhouWarriors PRIVATE
	../lib
	lib
)
target_link_libraries(TouhouWarriors
	x86/EchoEngine
	x86/SDL2
	x86/SDL2_image
	x86/SDL2_ttf
	x86/SDL2main
)
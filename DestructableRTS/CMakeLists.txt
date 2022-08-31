cmake_minimum_required(VERSION 3.19)
project(DestructableRTS)


add_executable(DestructableRTS
	main.cpp
 "Systems/SystemQueSpawn.h" "Systems/SystemUtilities.h" "Systems/SystemMoveTo.h" "Systems/SystemUnitAI.h")
target_include_directories(DestructableRTS PRIVATE
	../include
	include
)
target_link_directories(DestructableRTS PRIVATE
	../lib
	lib 
)
target_link_libraries(DestructableRTS
	x86/EchoEngine
	x86/SDL2
	x86/SDL2_image
	x86/SDL2_ttf
	x86/SDL2main
)

#add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")


IF(CMAKE_BUILD_TYPE MATCHES x86-Testing)
    #target_compile_definitions(RTS PUBLIC TESTING)
ENDIF(CMAKE_BUILD_TYPE MATCHES x86-Testing)
target_compile_definitions(DestructableRTS PUBLIC TESTING)



#SET_TARGET_PROPERTIES(RTS PROPERTIES LINK_FLAGS "/PROFILE")  # Not sure this works
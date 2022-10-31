cmake_minimum_required(VERSION 3.19)
project(EchoEngine)

add_library(EchoEngine STATIC
	Asset.cpp
	Widget.cpp
	Name.cpp
	profiler.cpp
	BitBool/BitBool.cpp
	IO_API/IO_API.c
	IO_API/GUI/GUI.c
)
if(DEFINED APP_TYPE_3D)
	target_compile_definitions(EchoEngine PRIVATE APP_TYPE_3D=${APP_TYPE_3D})
endif()

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_image)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_ttf)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/Chipmunk2D)

#Magnum specific
##find_package(Magnum REQUIRED GL Sdl2Application)
#set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)
##add_executable(MyApplication MyApplication.cpp)
#target_link_libraries(EchoEngine PRIVATE
#    Magnum::Application
#    Magnum::GL
#    Magnum::Magnum
#	Corrade
#)
# Make the executable a default target to build & run in Visual Studio
#set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyApplication)
#end

set(MAGNUM_WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/depsVisible/magnum EXCLUDE_FROM_ALL)

target_include_directories(EchoEngine PRIVATE
	deps
	deps/SDL2/include
	${CMAKE_CURRENT_SOURCE_DIR}
	deps/Chipmunk2D/include
	#depsVisible/magnum/magnum/src
	#depsVisible/magnum/corrade/src
)
target_link_libraries(EchoEngine
	SDL2
	SDL2_image
	SDL2_ttf
	SDL2main
	#SDL2-static
	chipmunk
	Magnum
	#Corrade
)



#IF (WIN32)
#	add_custom_command(TARGET EchoEngine POST_BUILD
#	COMMAND xcopy /s /d /y 
#		#"\"$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2/$<CONFIG>\""
#		#"\"$<TARGET_FILE_DIR:EchoEngine>/../*\""
#		"\"${CMAKE_BINARY_DIR}/deps/SDL2/$<CONFIG>\""
#		"\"${CMAKE_BINARY_DIR}/*\""
#)
#add_custom_command(TARGET EchoEngine POST_BUILD
#	COMMAND xcopy /s /d /y 
#		#"\"$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_image/$<CONFIG>\""
#		#"\"$<TARGET_FILE_DIR:EchoEngine>/../*\""
#		"\"${CMAKE_BINARY_DIR}/deps/SDL2_image/$<CONFIG>\""
#		"\"${CMAKE_BINARY_DIR}/*\""
#)
#add_custom_command(TARGET EchoEngine POST_BUILD
#	COMMAND xcopy /s /d /y 
#		#"\"$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_ttf/$<CONFIG>\""
#		#"\"$<TARGET_FILE_DIR:EchoEngine>/../*\""
#		"\"${CMAKE_BINARY_DIR}/deps/SDL2_ttf/$<CONFIG>\""
#		"\"${CMAKE_BINARY_DIR}/*\""
#)
#ELSE()
#	add_custom_command(TARGET EchoEngine POST_BUILD
#	COMMAND cp -u 
#		"\"$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2/$<CONFIG>\""
#		"\"$<TARGET_FILE_DIR:EchoEngine>/../*\""
#)
#add_custom_command(TARGET EchoEngine POST_BUILD
#	COMMAND cp -u 
#		"\"$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_image/$<CONFIG>\""
#		"\"$<TARGET_FILE_DIR:EchoEngine>/../*\""
#)
#add_custom_command(TARGET EchoEngine POST_BUILD
#	COMMAND cp -u 
#		"\"$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_ttf/$<CONFIG>\""
#		"\"$<TARGET_FILE_DIR:EchoEngine>/../*\""
#)
#ENDIF()



#add_custom_command(TARGET EchoEngine POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#		# "${CMAKE_BINARY_DIR}/deps/SDL2/${CMAKE_BUILD_TYPE}"
#		$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2/${CMAKE_BUILD_TYPE}
#		$<TARGET_FILE_DIR:EchoEngine>/../
#		#"${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
#)
#add_custom_command(TARGET EchoEngine POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#		#"${CMAKE_BINARY_DIR}/deps/SDL2_image/${CMAKE_BUILD_TYPE}"
#		$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_image/${CMAKE_BUILD_TYPE}
#		$<TARGET_FILE_DIR:EchoEngine>/../
#		#"${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
#)
#add_custom_command(TARGET EchoEngine POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#		#"${CMAKE_BINARY_DIR}/deps/SDL2_ttf/${CMAKE_BUILD_TYPE}"
#		$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_ttf/${CMAKE_BUILD_TYPE}
#		$<TARGET_FILE_DIR:EchoEngine>/../
#		#"${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
#)
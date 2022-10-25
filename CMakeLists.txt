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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_image)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_ttf)
target_include_directories(EchoEngine PRIVATE
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



add_custom_command(TARGET EchoEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
		# "${CMAKE_BINARY_DIR}/deps/SDL2/${CMAKE_BUILD_TYPE}"
		$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2/${CMAKE_BUILD_TYPE}
		$<TARGET_FILE_DIR:EchoEngine>/../
		#"${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
)
add_custom_command(TARGET EchoEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
		#"${CMAKE_BINARY_DIR}/deps/SDL2_image/${CMAKE_BUILD_TYPE}"
		$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_image/${CMAKE_BUILD_TYPE}
		$<TARGET_FILE_DIR:EchoEngine>/../
		#"${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
)
add_custom_command(TARGET EchoEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
		#"${CMAKE_BINARY_DIR}/deps/SDL2_ttf/${CMAKE_BUILD_TYPE}"
		$<TARGET_FILE_DIR:EchoEngine>/../deps/SDL2_ttf/${CMAKE_BUILD_TYPE}
		$<TARGET_FILE_DIR:EchoEngine>/../
		#"${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
)
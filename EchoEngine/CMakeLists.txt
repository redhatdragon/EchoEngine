cmake_minimum_required(VERSION 3.19)
project(EchoEngine)

add_library(EchoEngine STATIC
	Asset.cpp
	Widget.cpp
	Name.cpp
	profiler.cpp
	BitBool/BitBool.cpp
	../include/IO_API/IO_API.c
	../include/IO_API/GUI/GUI.c
  )
target_include_directories(EchoEngine PRIVATE
	../include
	include
)
target_link_directories(EchoEngine PRIVATE
	../lib
	lib
)
target_link_libraries(EchoEngine
	x86/SDL2
	x86/SDL2_image
	x86/SDL2_ttf
	x86/SDL2main
)



# Had to disable making a test executable directly due to compiler's heap overflowing.
# Consider moving test executable to a different project directory and automating static lib movement'

#add_executable(EchoEngineExe
#	Asset.cpp
#	Widget.cpp
#	name.cpp
#	profiler.cpp
#	BitBool/BitBool.cpp
#	main.cpp
#	../include/IO_API/IO_API.c
#	../include/IO_API/GUI/GUI.c
#	#"Systems/systemAI.cpp" "Systems/systemDamage.cpp" "Systems/systemDead.cpp" "Systems/systemDisplay.cpp" "Systems/systemPlayer.cpp" "Systems/systems.cpp"
 #)
#target_include_directories(EchoEngineExe PRIVATE
#	../include
#	include
#)
#target_link_directories(EchoEngineExe PRIVATE
#	../lib
#	lib
#)
#target_link_libraries(EchoEngineExe
#	x86/SDL2
#	x86/SDL2_image
#	x86/SDL2_ttf
#	x86/SDL2main
#)
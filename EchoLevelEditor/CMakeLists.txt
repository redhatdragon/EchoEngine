cmake_minimum_required(VERSION 3.19)
project(EchoLevelEditor)

link_directories(
	../lib
)

add_executable(EchoLevelEditor 
	main.cpp
	#Systems/systemAI.cpp
	#Systems/systemDamage.cpp
	#Systems/systemDead.cpp
	#Systems/systemDisplay.cpp
	#Systems/systemPlayer.cpp
	#Systems/systems.cpp
	#IO_API/SDL2/ThreadPool.cpp
 "UI/ArrowButton.h" "UI/UIUtility.h" "UI/TextBox.cpp" "UI/ArrowButton.cpp" "UI/TextFieldUint.cpp")

target_include_directories(EchoLevelEditor PRIVATE
	../include
	include
)

target_link_directories(EchoLevelEditor PUBLIC
	../lib
	lib
)

#target_link_libraries(EchoLevelEditor ../EchoEngine/out/build/x64-Debug/EchoEngine)
target_link_libraries(EchoLevelEditor
	x86/EchoEngine
	x86/SDL2
	x86/SDL2_image
	x86/SDL2_ttf
	x86/SDL2main
)
#target_link_libraries(EchoLevelEditor C:/Users/Yuki/source/repos/EchoEngineCmakePort/EchoLevelEditor/EchoEngine)
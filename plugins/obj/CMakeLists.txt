# -G "MinGW Makefiles"

cmake_minimum_required(VERSION 3.0)

set(COMMON_PARAMETERS "-I../../3rd/gl3w/ -I../../inc -I../../3rd/imgui/ -I../../3rd/krgui/ -DUNICODE -D_WIN32 -D_WIN64 -D__MINGW_EXTENSION")

set(CMAKE_GENERATOR "MinGW Makefiles")
set(CMAKE_CXX_FLAGS "${COMMON_PARAMETERS} -std=c++20 -O3")
set(CMAKE_C_FLAGS "${COMMON_PARAMETERS}")

SET(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc.exe)
SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++.exe)

project(OBJ)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin64-mingw/stdplugins/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../bin64-mingw/stdplugins/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../bin64-mingw/stdplugins/)
link_directories(../../lib/x64-mingw/)

add_library(
	OBJ SHARED
	export.cpp  
	import.cpp
	main.cpp
)

target_link_libraries(OBJ 
	PUBLIC 
	libkkrooo.engine.dll.a
)

set_target_properties(OBJ
	PROPERTIES 
	PREFIX ""
	SUFFIX ".kkp"
)
cmake_minimum_required(VERSION 3.0)

set(COMMON_PARAMETERS "-fPIC -I../../inc/ -I../../3rd/krgui/ -DUNICODE -D_WIN32 -D_WIN64")

set(CMAKE_GENERATOR "MinGW Makefiles")
set(CMAKE_CXX_FLAGS "${COMMON_PARAMETERS} -std=gnu++20 -O3")
set(CMAKE_C_FLAGS "${COMMON_PARAMETERS}")

project(AI_3Ds)

set(MINGW_DIR "e:/msys64/mingw64")
link_directories(${MINGW_DIR}/lib)
include_directories(${MINGW_DIR}/include)

SET(CMAKE_C_COMPILER ${MINGW_DIR}/bin/x86_64-w64-mingw32-gcc.exe)
SET(CMAKE_CXX_COMPILER ${MINGW_DIR}/bin/x86_64-w64-mingw32-g++.exe)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin64-mingw/stdplugins/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../bin64-mingw/stdplugins/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../bin64-mingw/stdplugins/)

add_library(
  AI_3ds SHARED 
  import.cpp
  main.cpp
)

target_link_libraries(AI_3ds -lassimp.dll)

set_target_properties(
	AI_3ds
	PROPERTIES 
	PREFIX ""
	SUFFIX ".kkp"
)
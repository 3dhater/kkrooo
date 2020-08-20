cmake_minimum_required(VERSION 3.0)

set(COMMON_PARAMETERS "-I../../3rd/gl3w/ -I../../inc -I../../3rd/imgui/ -I../../3rd/krgui/ -I../../3rd/zstd/lib/common/ -DUNICODE -D_WIN32 -D_WIN64 -D__MINGW_EXTENSION")

set(CMAKE_GENERATOR "MinGW Makefiles")
set(CMAKE_CXX_FLAGS "${COMMON_PARAMETERS} -std=c++20 -O3")
set(CMAKE_C_FLAGS "${COMMON_PARAMETERS}")

project(kkrooo.engine)

#set(MINGW_DIR "e:/msys64/mingw64")
#link_directories(${MINGW_DIR}/lib)
#include_directories(${MINGW_DIR}/include)
SET(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc.exe)
SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++.exe)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin64-mingw/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../lib/x64-mingw/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib/x64-mingw/)

add_library(
  kkrooo.engine SHARED 
  main_kkrooo.cpp
  ../../3rd/gl3w/GL/gl3w.c
  ../../3rd/zstd/lib/common/debug.c
  ../../3rd/zstd/lib/common/entropy_common.c
  ../../3rd/zstd/lib/common/error_private.c
  ../../3rd/zstd/lib/compress/fse_compress.c
  ../../3rd/zstd/lib/common/fse_decompress.c
  ../../3rd/zstd/lib/compress/hist.c
  ../../3rd/zstd/lib/compress/huf_compress.c
  ../../3rd/zstd/lib/decompress/huf_decompress.c
  ../../3rd/zstd/lib/common/pool.c
  ../../3rd/zstd/lib/common/threading.c
  ../../3rd/zstd/lib/common/xxhash.c
  ../../3rd/zstd/lib/common/zstd_common.c
  ../../3rd/zstd/lib/compress/zstd_compress.c
  ../../3rd/zstd/lib/compress/zstd_compress_literals.c
  ../../3rd/zstd/lib/compress/zstd_compress_sequences.c
  ../../3rd/zstd/lib/decompress/zstd_ddict.c
  ../../3rd/zstd/lib/decompress/zstd_decompress.c
  ../../3rd/zstd/lib/decompress/zstd_decompress_block.c
  ../../3rd/zstd/lib/compress/zstd_double_fast.c
  ../../3rd/zstd/lib/compress/zstd_fast.c
  ../../3rd/zstd/lib/compress/zstd_lazy.c
  ../../3rd/zstd/lib/compress/zstd_ldm.c
  ../../3rd/zstd/lib/compress/zstd_opt.c
  ../../3rd/zstd/lib/compress/zstdmt_compress.c
  kkEasyFunctionsImpl.cpp
  EventSystem/kkEventSystemImpl.cpp
  FileSystem/kkFileSystemCommon.cpp
  FileSystem/kkFileSystemWin32.cpp
  FileSystem/kkFileWin32.cpp
  FileSystem/kkXMLDocumentImpl.cpp
  FileSystem/kkXMLImpl.cpp
  GraphicsSystem/kkGraphicsSystemImpl.cpp
  GraphicsSystem/kkOpenGL.cpp
  GraphicsSystem/kkOpenGLModel.cpp
  GraphicsSystem/kkOpenGLShader.cpp
  GraphicsSystem/kkOpenGLTexture.cpp
  GraphicsSystem/kkOpenGLWin32.cpp
  InputSystem/kkInputSystemImpl.cpp
  MainSystem/kkMainSystemImpl.cpp
  MainSystem/kkMainSystemImpl_Win32.cpp
  Memory/kkMemory.cpp
  Memory/kkMemorySystemImpl.cpp
  SceneSystem/kkCameraImpl.cpp
  SceneSystem/kkSceneSystemImpl.cpp
  Window/kkWindowWin32.cpp  
)

target_link_libraries(
  kkrooo.engine 
  -lopengl32
  -lgdi32
)

set_target_properties(kkrooo.engine 
	PROPERTIES 
	PREFIX ""
)
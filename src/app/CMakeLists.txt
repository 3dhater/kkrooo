cmake_minimum_required(VERSION 3.0)

set(COMMON_PARAMETERS "-I../../3rd/gl3w/ -I../../inc -I../../3rd/imgui/ -I../../3rd/krgui/ -DUNICODE -D_WIN32 -D_WIN64 -D__MINGW_EXTENSION")

set(CMAKE_GENERATOR "MinGW Makefiles")
set(CMAKE_CXX_FLAGS "${COMMON_PARAMETERS} -std=c++20 -O3")
set(CMAKE_C_FLAGS "${COMMON_PARAMETERS}")

#set(MINGW_DIR "e:/msys64/mingw64")
#link_directories(${MINGW_DIR}/lib)
#include_directories(	${MINGW_DIR}/include)
#SET(CMAKE_C_COMPILER ${MINGW_DIR}/bin/x86_64-w64-mingw32-gcc.exe)
#SET(CMAKE_CXX_COMPILER ${MINGW_DIR}/bin/x86_64-w64-mingw32-g++.exe)
SET(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc.exe)
SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++.exe)

project(kkrooo)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin64-mingw/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../lib/x64-mingw/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib/x64-mingw/)
link_directories(../../lib/x64-mingw/)

add_executable(
  kkrooo WIN32
  ../../3rd/boost/libs/filesystem/src/codecvt_error_category.cpp
  ../../3rd/boost/libs/filesystem/src/directory.cpp
  ../../3rd/boost/libs/filesystem/src/exception.cpp
  ../../3rd/boost/libs/filesystem/src/operations.cpp
  ../../3rd/boost/libs/filesystem/src/path.cpp
  ../../3rd/boost/libs/filesystem/src/path_traits.cpp
  ../../3rd/boost/libs/filesystem/src/portability.cpp
  ../../3rd/boost/libs/filesystem/src/unique_path.cpp
  ../../3rd/boost/libs/filesystem/src/utf8_codecvt_facet.cpp
  ../../3rd/boost/libs/filesystem/src/windows_file_codecvt.cpp
  ../../3rd/imgui/imgui.cpp
  ../../3rd/imgui/imgui_draw.cpp
  ../../3rd/imgui/imgui_impl_opengl3.cpp
  ../../3rd/imgui/imgui_impl_win32.cpp
  ../../3rd/imgui/imgui_widgets.cpp
  ../../3rd/gl3w/GL/gl3w.c
  ../../3rd/krgui/krGui.cpp
  Application.cpp
  EventConsumer.cpp
  Fonts.cpp
  Functions.cpp
  Gizmo.cpp
  main_app.cpp
  ShortcutManager.cpp
  window_callbacks.cpp
  Geometry/GeometryCreator.cpp
  Geometry/Polygon3D.cpp
  Geometry/PolygonalModel.cpp
  Geometry/Vertex.cpp
  GUI/DrawRightPart.cpp
  GUI/GUIResources.cpp
  GUI/MainMenu_objectCreatorCategories.cpp
  GUI/MainMenuBar.cpp
  GUI/MainMenuCommands.cpp
  Materials/kkMaterialTypeImpl.cpp
  Materials/MaterialEditor.cpp
  Plugins/Plugin.cpp
  Plugins/PluginCommonInterface.cpp
  Plugins/PluginGUI.cpp
  Plugins/PluginGUIWindow.cpp
  Plugins/PluginManager.cpp
  Plugins/PluginObjectCategory.cpp
  RenderManager/RenderManager.cpp
  Scene3D/Scene3D.cpp
  Scene3D/Scene3DObject.cpp
  Shaders/points.cpp
  Shaders/scene3DobjectDefault.cpp
  Shaders/simple.cpp
  Viewport/Viewport.cpp
  Viewport/ViewportCamera.cpp
  Viewport/ViewportCameraUpdateCallback.cpp
  Viewport/ViewportOptimizations.cpp
)


target_link_libraries(kkrooo 
	PUBLIC 
	-lopengl32 
	-lole32 
	-luser32 
	-lcomdlg32 
	-luuid 
	libkkrooo.engine.dll.a
)

#target_link_libraries(kkrooo -lopengl32)

set_target_properties(kkrooo PROPERTIES PREFIX "")
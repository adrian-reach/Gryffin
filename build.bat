@echo off
set INCLUDE_FLAGS=-Iinclude -Iinclude/imgui -Iinclude/imgui/backends -Iinclude/SDL2 -Iinclude/glm -Iinclude/ffmpeg -Iinclude/glad -Iinclude/KHR -Iinclude/src
set LIB_FLAGS=-Llib -lSDL2 -lavcodec -lavformat -lswscale -lswresample -lavutil -lopengl32 -lgdi32 -luser32 -lws2_32 -lz -lwinhttp -lstdc++ -lm -L"C:\Program Files\OpenSSL-Win64\lib\VC\MD"

REM Ensure shader directory exists
if not exist "src\shaders" mkdir "src\shaders"

REM Build the executable
g++ -g -O0 -o doomlike ^
src/helpers/*.cpp ^
src/renderer/*.cpp ^
src/engine/*.cpp ^
src/engine/components/light.cpp ^
src/engine/components/meshrenderer.cpp ^
src/*.cpp ^
include/glad/glad.c ^
include/imgui/imgui.cpp ^
include/imgui/imgui_draw.cpp ^
include/imgui/imgui_widgets.cpp ^
include/imgui/imgui_tables.cpp ^
include/imgui/backends/imgui_impl_sdl2.cpp ^
include/imgui/backends/imgui_impl_opengl3.cpp ^
%INCLUDE_FLAGS% %LIB_FLAGS%

REM Copy shader files to build directory
xcopy /y /i src\shaders\*.* .

@echo off
set INCLUDE_FLAGS=-Iinclude -Iinclude/imgui -Iinclude/imgui/backends -Iinclude/SDL2 -Iinclude/glm -Iinclude/ffmpeg -Iinclude/glad -Iinclude/KHR -Iinclude/src -Iinclude/ImGuizmo
set LIB_FLAGS=-Llib -lSDL2 -lavcodec -lavformat -lswscale -lswresample -lavutil -lopengl32 -lgdi32 -luser32 -lws2_32 -lz -lwinhttp -lstdc++ -lm -L"C:\Program Files\OpenSSL-Win64\lib\VC\MD"

REM Ensure shader directory exists
if not exist "src\shaders" mkdir "src\shaders"

REM Build the executable
set SUB_DIRECTORY_SOURCE_FILES =
for %%f in (src/engine/game_objects/*.cpp) do set SOURCE_FILES=!SOURCE_FILES! %%f
for %%f in (src/engine/components/*.cpp) do set SOURCE_FILES=!SOURCE_FILES! %%f

@REM src/engine/components/light.cpp ^
@REM src/engine/components/meshrenderer.cpp ^


REM Build the executable
g++ -g -O0 -o editor ^
src/helpers/*.cpp ^
src/renderer/*.cpp ^
src/engine/*.cpp ^
%SUB_DIRECTORY_SOURCE_FILES % ^
src/*.cpp ^
include/glad/glad.c ^
include/imgui/imgui.cpp ^
include/imgui/imgui_draw.cpp ^
include/imgui/imgui_widgets.cpp ^
include/imgui/imgui_tables.cpp ^
include/imgui/backends/imgui_impl_sdl2.cpp ^
include/imgui/backends/imgui_impl_opengl3.cpp ^
include/ImGuizmo/ImGuizmo.cpp ^
%INCLUDE_FLAGS% %LIB_FLAGS%

@echo off
setlocal enabledelayedexpansion

REM Clean previous version info
if exist version.txt del version.txt

REM Version increment logic
if not exist version_number.txt (
    echo 1 > version_number.txt
) else (
    for /f %%i in (version_number.txt) do (
        set /a "new_version=%%i+1"
        echo !new_version! > version_number.txt
    )
)

REM Generate version file
git rev-parse --short HEAD >> version.txt
git rev-parse --abbrev-ref HEAD >> version.txt
type version_number.txt >> version.txt

REM Rest of build commands...

REM Copy shader files to build directory
xcopy /y /i src\shaders\*.* .

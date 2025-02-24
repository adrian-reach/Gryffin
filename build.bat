@echo off
setlocal enabledelayedexpansion

REM Set paths
set VCPKG_ROOT=X:\Projects\vcpkg
set INCLUDE_FLAGS=-Iinclude -Iinclude/sol2 -I%VCPKG_ROOT%\installed\x64-windows\include -Iinclude/imgui -Iinclude/imgui/backends -Iinclude/SDL2 -Iinclude/glm -Iinclude/ffmpeg -Iinclude/glad -Iinclude/KHR -Iinclude/src -Iinclude/ImGuizmo -Iinclude/json
set LIB_FLAGS=-Llib -L%VCPKG_ROOT%\installed\x64-windows\lib -llua -lSDL2 -lavcodec -lavformat -lswscale -lswresample -lavutil -lopengl32 -lgdi32 -luser32 -lws2_32 -lz -lwinhttp -lstdc++ -lm -L"C:\Program Files\OpenSSL-Win64\lib\VC\MD"

REM Ensure shader directory exists
if not exist "src\shaders" mkdir "src\shaders"

REM Build the executable
set SUB_DIRECTORY_SOURCE_FILES =
for %%f in (src/engine/game_objects/*.cpp) do set SOURCE_FILES=!SOURCE_FILES! %%f
for %%f in (src/engine/components/*.cpp) do set SOURCE_FILES=!SOURCE_FILES! %%f

REM Build the executable
g++ -g -O0 -std=c++2a -fpermissive -w -Wfatal-errors -fmax-errors=1 -D_GLIBCXX_USE_CXX11_ABI=1 -o editor ^
src/helpers/*.cpp ^
src/renderer/*.cpp ^
src/engine/*.cpp ^
%SUB_DIRECTORY_SOURCE_FILES% ^
src/*.cpp ^
include/glad/glad.c ^
include/imgui/imgui.cpp ^
include/imgui/imgui_draw.cpp ^
include/imgui/imgui_widgets.cpp ^
include/imgui/imgui_tables.cpp ^
include/imgui/backends/imgui_impl_sdl2.cpp ^
include/imgui/backends/imgui_impl_opengl3.cpp ^
include/ImGuizmo/ImGuizmo.cpp ^
src/engine/components/light.cpp ^
src/engine/components/meshrenderer.cpp ^
src/engine/components/script_component.cpp ^
src/engine/scripting/lua_context.cpp ^
src/engine/scripting/lua_binding.cpp ^
%INCLUDE_FLAGS% %LIB_FLAGS%

if %errorlevel% neq 0 (
    echo Build failed with error %errorlevel%
    exit /b %errorlevel%
)

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

echo Build completed successfully!

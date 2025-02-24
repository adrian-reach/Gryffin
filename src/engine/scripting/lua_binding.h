#pragma once

// Sol2 configuration
#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 1

#include <sol/sol.hpp>
#include "../gameobject.h"
#include "../components/transform_component.h"

class LuaBinding
{
public:
    static void BindEngineAPI(lua_State *L, GameObject *owner);
    static void BindTransformComponent(sol::state &lua);

private:
    static int GetTransform(lua_State *L);
    static int LogDebug(lua_State *L);
    static int GetDeltaTime(lua_State *L);
};

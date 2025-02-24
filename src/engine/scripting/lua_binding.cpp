// src/engine/scripting/lua_binding.cpp
#include "lua_binding.h"
#include "../components/transform_component.h"

void LuaBinding::BindTransformComponent(sol::state &lua)
{
    lua.new_usertype<TransformComponent>("TransformComponent",
        "getEulerAngles", &TransformComponent::getEulerAngles,
        "setEulerAngles", &TransformComponent::setEulerAngles
    );
}

void LuaBinding::BindEngineAPI(lua_State *L, GameObject *owner)
{
    // TODO: Implement engine API binding
}

int LuaBinding::GetTransform(lua_State *L)
{
    // TODO: Implement get transform
    return 0;
}

int LuaBinding::LogDebug(lua_State *L)
{
    // TODO: Implement log debug
    return 0;
}

int LuaBinding::GetDeltaTime(lua_State *L)
{
    // TODO: Implement get delta time
    return 0;
}

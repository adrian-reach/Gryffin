// src/engine/scripting/lua_context.cpp
#include "lua_context.h"
#include "lua_binding.h"
#include <fstream>

LuaContext::LuaContext()
{
    m_lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::coroutine,
        sol::lib::string,
        sol::lib::os,
        sol::lib::math,
        sol::lib::table,
        sol::lib::debug,
        sol::lib::bit32,
        sol::lib::io
    );
}

LuaContext::~LuaContext() = default;

bool LuaContext::LoadScript(const std::string& path)
{
    try
    {
        auto result = m_lua.safe_script_file(path);
        return result.valid();
    }
    catch (const sol::error& e)
    {
        // Handle error
        return false;
    }
}

void LuaContext::CallFunction(const std::string& name)
{
    try
    {
        sol::function fn = m_lua[name];
        if (fn.valid())
        {
            fn();
        }
    }
    catch (const sol::error& e)
    {
        // Handle error
    }
}

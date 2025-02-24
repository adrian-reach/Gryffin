// src/engine/scripting/lua_context.h
#pragma once

// Sol2 configuration
#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 1

#include <sol/sol.hpp>
#include <string>

class LuaContext
{
public:
    LuaContext();
    ~LuaContext();

    bool LoadScript(const std::string &path);
    void CallFunction(const std::string &name);

    template <typename... Args>
    void CallFunction(const std::string &name, Args &&...args)
    {
        auto fn = m_lua[name];
        if (fn.valid())
        {
            fn(std::forward<Args>(args)...);
        }
    }

    lua_State *L() { return m_lua.lua_state(); }
    sol::state &State() { return m_lua; }

private:
    sol::state m_lua;
    std::string m_lastError;
};
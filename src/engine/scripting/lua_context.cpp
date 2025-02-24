// src/engine/scripting/lua_context.cpp
#include "lua_context.h"
#include "lua_binding.h"
#include "../components/transform_component.h"
#include "../components/script_component.h"
#include "../gameobject.h"
#include "../../helpers/logging.h"
#include <fstream>

LuaContext::LuaContext() : m_scriptComponent(nullptr)
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
        sol::lib::io);
    
    LOG_INFO("Lua context initialized");
}

LuaContext::~LuaContext() = default;

bool LuaContext::LoadScript(const std::string &path)
{
    try
    {
        LOG_INFO("Loading script: {}", path);
        
        // Bind the transform component
        m_lua.new_usertype<TransformComponent>("TransformComponent",
            "getEulerAngles", &TransformComponent::getEulerAngles,
            "setEulerAngles", &TransformComponent::setEulerAngles
        );

        // Expose the transform to the script
        if (m_scriptComponent)
        {
            if (auto* owner = m_scriptComponent->getOwner())
            {
                if (auto* transform = owner->getComponent<TransformComponent>())
                {
                    m_lua["transform"] = transform;
                    LOG_INFO("Transform component bound to Lua");
                }
                else
                {
                    LOG_ERROR("No transform component found on owner");
                    return false;
                }
            }
            else
            {
                LOG_ERROR("Script component has no owner");
                return false;
            }
        }
        else
        {
            LOG_ERROR("No script component set");
            return false;
        }

        auto result = m_lua.safe_script_file(path);
        if (!result.valid())
        {
            sol::error err = result;
            LOG_ERROR("Failed to load script: {}", err.what());
            return false;
        }

        LOG_INFO("Script loaded successfully");
        return true;
    }
    catch (const sol::error &e)
    {
        LOG_ERROR("Error loading script: {}", e.what());
        return false;
    }
}

void LuaContext::CallFunction(const std::string &name)
{
    try
    {
        sol::function fn = m_lua[name];
        if (fn.valid())
        {
            fn();
            LOG_DEBUG("Called Lua function: {}", name);
        }
        else
        {
            LOG_WARNING("Lua function not found: {}", name);
        }
    }
    catch (const sol::error &e)
    {
        LOG_ERROR("Error calling Lua function {}: {}", name, e.what());
    }
}

// src/engine/components/script_component.cpp
#include "script_component.h"
#include "../scripting/lua_context.h"
#include "../components/transform_component.h"
#include "../../helpers/logging.h"
#include <memory>
#include <fstream>

ScriptComponent::ScriptComponent() = default;
ScriptComponent::~ScriptComponent() = default;

void ScriptComponent::Start()
{
    if (scriptPath.empty())
    {
        LOG_WARNING("Script path is empty");
        return;
    }

    // Check if file exists
    std::ifstream f(scriptPath.c_str());
    if (!f.good())
    {
        LOG_ERROR("Script file not found: {}", scriptPath);
        return;
    }

    LOG_INFO("Creating Lua context for script: {}", scriptPath);
    m_context = std::make_unique<LuaContext>();
    m_context->setScriptComponent(this);
    
    if (!m_context->LoadScript(scriptPath))
    {
        LOG_ERROR("Failed to load script: {}", scriptPath);
        return;
    }

    try
    {
        LOG_INFO("Calling Start() function in script");
        m_context->CallFunction("Start");
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Error in script Start(): {}", e.what());
    }
}

void ScriptComponent::Update(float deltaTime)
{
    if (!m_context)
        return;

    try
    {
        m_context->CallFunction("Update", deltaTime);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Error in script Update(): {}", e.what());
    }
}
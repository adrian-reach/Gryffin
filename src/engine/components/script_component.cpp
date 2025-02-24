// src/engine/components/script_component.cpp
#include "script_component.h"
#include "../scripting/lua_context.h"
#include <memory>
#include <fstream>

void ScriptComponent::Start()
{
    if (scriptPath.empty())
        return;

    // Check if file exists
    std::ifstream f(scriptPath.c_str());
    if (!f.good())
        return;

    m_context = std::make_unique<LuaContext>();
    if (!m_context->LoadScript(scriptPath))
    {
        // Handle error
        return;
    }

    try
    {
        m_context->CallFunction("Start");
    }
    catch (const std::exception& e)
    {
        // Handle error
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
        // Handle error
    }
}
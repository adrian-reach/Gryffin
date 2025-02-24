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
    catch (const std::exception &e)
    {
        LOG_ERROR("Error in script Start(): {}", e.what());
    }
}

void ScriptComponent::serialize(json &j) const
{
    Component::serialize(j);
    j["scriptPath"] = scriptPath;
}

void ScriptComponent::deserialize(const json &j)
{
    Component::deserialize(j);
    
    // Handle the case where scriptPath might be a number or other type
    if (j.contains("scriptPath")) {
        try {
            if (j["scriptPath"].is_string()) {
                scriptPath = j["scriptPath"].get<std::string>();
            } else {
                // Convert non-string value to string
                scriptPath = j["scriptPath"].dump();
                LOG_WARNING("ScriptComponent: scriptPath was not a string, converted value: {}", scriptPath);
            }
        } catch (const json::exception& e) {
            LOG_ERROR("Failed to deserialize scriptPath: {}", e.what());
            scriptPath = "";
        }
    } else {
        scriptPath = "";
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
    catch (const std::exception &e)
    {
        LOG_ERROR("Error in script Update(): {}", e.what());
    }
}
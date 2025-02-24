#pragma once
#include "../component.h"
#include <string>
#include <memory>

class LuaContext;

class ScriptComponent : public Component
{
public:
    std::string scriptPath;

    void Update(float deltaTime) override;
    void Start() override;

    // Serialization
    template <typename Archive>
    void serialize(Archive &archive)
    {
        archive(scriptPath);
    }

private:
    std::unique_ptr<LuaContext> m_context;
};

#pragma once
#include "../component.h"
#include <string>
#include <memory>

// Forward declaration
class LuaContext;

class ScriptComponent : public Component
{
public:
    ScriptComponent();
    ~ScriptComponent();
    
    std::string scriptPath;

    void Update(float deltaTime) override;
    void Start() override;

    // Required override from Component base class
    std::string getTypeName() const override { return "ScriptComponent"; }

    // Serialization
    template <typename Archive>
    void serialize(Archive &archive)
    {
        archive(scriptPath);
    }

private:
    std::unique_ptr<LuaContext> m_context;
};

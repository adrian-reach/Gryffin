/**
 * @file component.h
 * @brief Component class for attaching functionality to GameObjects
 */

#pragma once
#include <string>
#include <memory>
#include "serialization.h"
#include "../renderer/shader.h"

// Forward declarations
class GameObject;
class Shader;

class Component : public ISerializable
{
protected:
    GameObject *owner;
    bool enabled;

public:
    Component() : owner(nullptr), enabled(true) {}
    virtual ~Component() = default;

    virtual void Start() {}
    virtual void Update(float deltaTime) {}
    virtual void Render(Shader &shader) {}
    virtual void OnGUI() {}

    void setOwner(GameObject *newOwner)
    {
        owner = newOwner;
    }

    GameObject *getOwner() const
    {
        return owner;
    }

    bool isEnabled() const
    {
        return enabled;
    }

    void setEnabled(bool value)
    {
        enabled = value;
    }

    virtual std::string getTypeName() const = 0;

    // Serialization
    virtual void serialize(json &j) const override
    {
        j["type"] = getTypeName();
        j["enabled"] = enabled;
    }

    virtual void deserialize(const json &j) override
    {
        enabled = j["enabled"].get<bool>();
    }
};

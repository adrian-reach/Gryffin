/**
 * @file component.h
 * @brief Component class for attaching functionality to GameObjects
 */

#pragma once
#include <string>
#include "iserializable.h"
#include "serialization.h"

class GameObject;
class Shader;

class Component : public ISerializable {
public:
    virtual ~Component() = default;
    
    // Core component interface
    virtual void start() {} // Called when component is first created
    virtual void update(float deltaTime) {}
    virtual void render(Shader& shader) {}
    virtual void onGUI() {}                // For editor property editing
    virtual std::string getTypeName() const = 0;

    // Owner management
    void setOwner(GameObject* owner) { this->owner = owner; }
    GameObject* getOwner() const { return owner; }

    // Enable/Disable component
    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) { enabled = value; }

    // Default serialization (override in derived classes)
    virtual void serialize(json& j) const override {
        j["type"] = getTypeName();
    }

    virtual void deserialize(const json& j) override {
        // Base component has no data to deserialize
    }

protected:
    GameObject* owner = nullptr;
    bool enabled = true;
};

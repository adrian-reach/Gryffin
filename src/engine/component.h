/**
 * @file component.h
 * @brief Component class for attaching functionality to GameObjects
 */

#pragma once
#include <string>

class GameObject;
class Shader;

class Component
{
public:
    Component() : owner(nullptr), enabled(true) {}
    virtual ~Component() = default;

    // Core component functions
    virtual void start() {} // Called when component is first created
    virtual void update(float deltaTime) {}
    virtual void render(Shader &shader) {} // Changed from const reference to allow shader modifications
    virtual void onGUI() {}                // For editor property editing

    // Component identification
    virtual std::string getTypeName() const = 0;

    // Owner management
    void setOwner(GameObject *newOwner) { owner = newOwner; }
    GameObject *getOwner() const { return owner; }

    // Enable/Disable component
    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) { enabled = value; }

protected:
    GameObject *owner;
    bool enabled;
};

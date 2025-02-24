#pragma once
#include "component.h"
#include "components/transform_component.h"
#include "components/meshrenderer.h"
#include "components/light.h"
#include "components/script_component.h"
#include "serialization.h"
#include <memory>
#include <vector>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <atomic>

class GameObject : public ISerializable
{
public:
    GameObject(const std::string &objectName = "GameObject")
        : id(nextId++), name(objectName), isStatic(false), isActive(true)
    {
        name = objectName + " (" + std::to_string(id) + ")";
        transformComponent = addComponent<TransformComponent>();
    }

    // Object properties
    uint64_t id; // Removed const to allow deserialization
    std::string name;
    bool isStatic;
    bool isActive;

    // Transform operations
    void setPosition(const glm::vec3 &pos)
    {
        if (transformComponent)
        {
            transformComponent->position = pos;
        }
    }

    void setRotation(const glm::vec3 &eulerAngles)
    {
        if (transformComponent)
        {
            transformComponent->setEulerAngles(eulerAngles);
        }
    }

    void setScale(const glm::vec3 &scl)
    {
        if (transformComponent)
        {
            transformComponent->scale = scl;
        }
    }

    // Getters
    glm::vec3 getPosition() const
    {
        return transformComponent ? transformComponent->position : glm::vec3(0.0f);
    }

    glm::vec3 getRotation() const
    {
        return transformComponent ? transformComponent->getEulerAngles() : glm::vec3(0.0f);
    }

    glm::vec3 getScale() const
    {
        return transformComponent ? transformComponent->scale : glm::vec3(1.0f);
    }

    glm::mat4 getModelMatrix() const
    {
        return transformComponent ? transformComponent->getLocalMatrix() : glm::mat4(1.0f);
    }

    TransformComponent *getTransform() const
    {
        return transformComponent;
    }

    // Component management
    template <typename T>
    T *addComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        auto component = std::make_unique<T>();
        T *componentPtr = component.get();
        component->setOwner(this);

        // Convert to base class pointer before moving into vector
        components.push_back(std::unique_ptr<Component>(component.release()));

        if (auto transform = dynamic_cast<TransformComponent *>(componentPtr))
        {
            transformComponent = transform;
        }

        return componentPtr;
    }

    template <typename T>
    T *getComponent() const
    {
        for (const auto &component : components)
        {
            if (auto typed = dynamic_cast<T *>(component.get()))
            {
                return typed;
            }
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Component>> &getAllComponents() const
    {
        return components;
    }

    void clearComponents()
    {
        components.clear();
        transformComponent = nullptr;
    }

    // Update
    void update(float deltaTime)
    {
        if (!isActive)
            return;
        for (const auto &component : components)
        {
            if (component->isEnabled())
            {
                component->Update(deltaTime);
            }
        }
    }

    // Serialization
    void serialize(json &j) const override
    {
        j["id"] = id;
        j["name"] = name;
        j["isStatic"] = isStatic;
        j["isActive"] = isActive;

        // Serialize components
        json componentsArray = json::array();
        for (const auto &component : components)
        {
            json componentJson;
            component->serialize(componentJson);
            componentsArray.push_back(componentJson);
        }
        j["components"] = componentsArray;
    }

    void deserialize(const json &j) override
    {
        // Clear existing components first
        clearComponents();

        // Deserialize basic properties
        id = j["id"].get<uint64_t>();
        name = j["name"].get<std::string>();
        isStatic = j["isStatic"].get<bool>();
        isActive = j["isActive"].get<bool>();

        // Deserialize components
        const auto &componentsArray = j["components"];
        for (const auto &componentJson : componentsArray)
        {
            std::string typeName = componentJson["type"].get<std::string>();

            std::unique_ptr<Component> component;

            // Create appropriate component type
            if (typeName == "TransformComponent")
            {
                component = std::make_unique<TransformComponent>();
                transformComponent = static_cast<TransformComponent *>(component.get());
            }
            else if (typeName == "MeshRenderer")
            {
                component = std::make_unique<MeshRenderer>();
            }
            else if (typeName == "Light")
            {
                component = std::make_unique<Light>();
            }
            else if (typeName == "ScriptComponent")
            {
                component = std::make_unique<ScriptComponent>();
            }

            if (component)
            {
                component->setOwner(this);
                component->deserialize(componentJson);
                components.push_back(std::move(component));
            }
        }

        // Ensure we have a transform component
        if (!transformComponent)
        {
            transformComponent = addComponent<TransformComponent>();
        }
    }

private:
    TransformComponent *transformComponent;
    std::vector<std::unique_ptr<Component>> components;
    static std::atomic<uint64_t> nextId;
};
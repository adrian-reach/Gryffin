#pragma once
#include "component.h"
#include "components/transform_component.h"
#include <memory>
#include <vector>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <atomic>

class GameObject {
public:
    GameObject(const std::string& objectName = "GameObject")
        : id(nextId++), name(objectName), isStatic(false), isActive(true)
    {
        name = objectName + " (" + std::to_string(id) + ")";
        transformComponent = addComponent<TransformComponent>();
    }

    // Object properties
    const uint64_t id;
    std::string name;
    bool isStatic;
    bool isActive;

    // Transform operations
    void setPosition(const glm::vec3& pos) {
        if (transformComponent) {
            transformComponent->position = pos;
        }
    }

    void setRotation(const glm::vec3& eulerAngles) {
        if (transformComponent) {
            transformComponent->setEulerAngles(eulerAngles);
        }
    }

    void setScale(const glm::vec3& scl) {
        if (transformComponent) {
            transformComponent->scale = scl;
        }
    }

    // Getters
    glm::vec3 getPosition() const {
        return transformComponent ? transformComponent->position : glm::vec3(0.0f);
    }

    glm::vec3 getRotation() const {
        return transformComponent ? transformComponent->getEulerAngles() : glm::vec3(0.0f);
    }

    glm::vec3 getScale() const {
        return transformComponent ? transformComponent->scale : glm::vec3(1.0f);
    }

    glm::mat4 getModelMatrix() const {
        return transformComponent ? transformComponent->getLocalMatrix() : glm::mat4(1.0f);
    }

    TransformComponent* getTransform() const {
        return transformComponent;
    }

    // Component management
    template <typename T>
    T *addComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto component = std::make_unique<T>();
        component->setOwner(this);
        component->start();

        T *componentPtr = component.get();
        components.push_back(std::move(component));
        return componentPtr;
    }

    template <typename T>
    T *getComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        for (const auto &component : components)
        {
            if (T *result = dynamic_cast<T *>(component.get()))
            {
                return result;
            }
        }
        return nullptr;
    }

    template <typename T>
    std::vector<T *> getComponents()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        std::vector<T *> result;
        for (const auto &component : components)
        {
            if (T *castResult = dynamic_cast<T *>(component.get()))
            {
                result.push_back(castResult);
            }
        }
        return result;
    }

    void removeComponent(Component *component)
    {
        auto it = std::find_if(components.begin(), components.end(),
                               [component](const std::unique_ptr<Component> &ptr)
                               {
                                   return ptr.get() == component;
                               });

        if (it != components.end())
        {
            components.erase(it);
        }
    }

    // Core functionality
    void update(float deltaTime)
    {
        if (!isActive)
            return;

        for (const auto &component : components)
        {
            if (component->isEnabled())
            {
                component->update(deltaTime);
            }
        }
    }

    void render(Shader &shader)
    {
        if (!isActive)
            return;

        for (const auto &component : components)
        {
            if (component->isEnabled())
            {
                component->render(shader);
            }
        }
    }

    void onGUI()
    {
        for (const auto &component : components)
        {
            if (component->isEnabled())
            {
                component->onGUI();
            }
        }
    }

    // Get all components
    const std::vector<std::unique_ptr<Component>> &getAllComponents() const
    {
        return components;
    }

private:
    TransformComponent* transformComponent;
    std::vector<std::unique_ptr<Component>> components;
    static inline std::atomic<uint64_t> nextId{0};
};
#pragma once
#include <string>

#include <glm/glm.hpp>
#include <imgui.h>

#include "../component.h"
#include "../gameobject.h"
#include "transform_component.h"

class Light : public Component
{
public:
    enum class Type
    {
        Point,
        Directional,
        Spot
    };

    Light()
        : type(Type::Point),
          color(1.0f, 1.0f, 1.0f),
          intensity(1.0f),
          range(10.0f),
          spotAngle(45.0f),
          castShadows(false)
    {
    }

    void render(Shader &shader) override
    {
        // TODO: Implement light rendering
        // Light properties are set by the Scene class during rendering
        // since we need to handle multiple lights and default lighting
    }

    void onGUI() override
    {
        // Light type selection
        const char *typeNames[] = {"Point", "Directional", "Spot"};
        int currentType = static_cast<int>(type);
        if (ImGui::Combo("Light Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames)))
        {
            type = static_cast<Type>(currentType);
        }

        // Position (using TransformComponent)
        if (auto transform = owner->getTransform())
        {
            glm::vec3 pos = transform->position;
            if (ImGui::DragFloat3("Position", &pos[0], 0.1f))
            {
                transform->position = pos;
            }
        }

        // Light properties
        ImGui::ColorEdit3("Color", &color[0]);
        ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 10.0f);

        if (type != Type::Directional)
        {
            ImGui::DragFloat("Range", &range, 0.5f, 0.1f, 100.0f);
        }

        if (type == Type::Spot)
        {
            ImGui::DragFloat("Spot Angle", &spotAngle, 1.0f, 1.0f, 179.0f);
        }

        ImGui::Checkbox("Cast Shadows", &castShadows);
    }

    std::string getTypeName() const override
    {
        return "Light";
    }

    // Light properties access
    Type getLightType() const { return type; }
    void setLightType(Type newType) { type = newType; }

    const glm::vec3 &getColor() const { return color; }
    void setColor(const glm::vec3 &newColor) { color = newColor; }

    float getIntensity() const { return intensity; }
    void setIntensity(float value) { intensity = value; }

    float getRange() const { return range; }
    void setRange(float value) { range = value; }

    float getSpotAngle() const { return spotAngle; }
    void setSpotAngle(float angle) { spotAngle = angle; }

    bool getCastShadows() const { return castShadows; }
    void setCastShadows(bool value) { castShadows = value; }

    // Get light direction (for directional and spot lights)
    glm::vec3 getDirection() const
    {
        if (auto transform = owner->getTransform())
        {
            return transform->forward();
        }
        return glm::vec3(0.0f, -1.0f, 0.0f); // Default down direction
    }

    // Get light position (for point and spot lights)
    glm::vec3 getPosition() const
    {
        if (auto transform = owner->getTransform())
        {
            return transform->position;
        }
        return glm::vec3(0.0f);
    }

private:
    Type type;
    glm::vec3 color;
    float intensity;
    float range;     // For point and spot lights
    float spotAngle; // For spot lights
    bool castShadows;
};

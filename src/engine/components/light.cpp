/**
 * @file light.cpp
 * @brief Light component for handling light sources
 */
#include "light.h"
#include "../gameobject.h"
#include "transform_component.h"
#include "imgui.h"

// Note: Most of the Light component's functionality is implemented in the header
// since it's primarily getters/setters and the core functionality is handled
// in the Scene's render method. This cpp file exists mainly for proper linking.

Light::Light()
    : type(Type::Directional), color(1.0f), intensity(1.0f), range(10.0f), spotAngle(45.0f)
{
}

void Light::Render(Shader &shader)
{
    // TODO: Set light uniforms in shader
    if (!enabled)
        return;

    auto transform = owner->getTransform();
    if (!transform)
        return;

    // Set light properties in shader
    shader.setVec3("lightColor", color);
    shader.setFloat("lightIntensity", intensity);

    if (type == Type::Directional)
    {
        shader.setVec3("lightDir", getDirection());
    }
    else
    {
        shader.setVec3("lightPos", getPosition());
        shader.setFloat("lightRange", range);

        if (type == Type::Spot)
        {
            shader.setFloat("spotAngle", glm::radians(spotAngle));
            shader.setVec3("spotDir", getDirection());
        }
    }
}

void Light::OnGUI()
{
    if (ImGui::TreeNode("Light"))
    {
        // Type selector
        const char *types[] = {"Directional", "Point", "Spot"};
        int currentType = static_cast<int>(type);
        if (ImGui::Combo("Type", &currentType, types, IM_ARRAYSIZE(types)))
        {
            type = static_cast<Type>(currentType);
        }

        // Color
        ImGui::ColorEdit3("Color", &color.x);

        // Intensity
        ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 10.0f);

        // Range (only for point and spot lights)
        if (type != Type::Directional)
        {
            ImGui::DragFloat("Range", &range, 0.5f, 0.1f, 100.0f);
        }

        // Spot angle (only for spot lights)
        if (type == Type::Spot)
        {
            ImGui::DragFloat("Spot Angle", &spotAngle, 1.0f, 1.0f, 179.0f);
        }

        // Show transform info
        if (auto transform = owner->getTransform())
        {
            if (type == Type::Directional)
            {
                glm::vec3 direction = getDirection();
                ImGui::Text("Direction: %.2f, %.2f, %.2f", direction.x, direction.y, direction.z);
            }
            else
            {
                glm::vec3 position = getPosition();
                ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
            }
        }

        ImGui::TreePop();
    }
}

glm::vec3 Light::getDirection() const
{
    if (auto transform = owner->getTransform())
    {
        // Use the forward vector of the transform
        glm::mat4 rotation = transform->getLocalMatrix();
        return -glm::normalize(glm::vec3(rotation[2])); // -Z is forward
    }
    return glm::vec3(0.0f, -1.0f, 0.0f); // Default to pointing down
}

glm::vec3 Light::getPosition() const
{
    if (auto transform = owner->getTransform())
    {
        return transform->position;
    }
    return glm::vec3(0.0f);
}

void Light::serialize(json &j) const
{
    Component::serialize(j);
    j["type"] = static_cast<int>(type);
    j["color"] = {color.r, color.g, color.b};
    j["intensity"] = intensity;
    j["range"] = range;
    j["spotAngle"] = spotAngle;
}

void Light::deserialize(const json &j)
{
    Component::deserialize(j);
    type = static_cast<Type>(j["type"].get<int>());
    auto colorArray = j["color"].get<std::vector<float>>();
    color = glm::vec3(colorArray[0], colorArray[1], colorArray[2]);
    intensity = j["intensity"].get<float>();
    range = j["range"].get<float>();
    spotAngle = j["spotAngle"].get<float>();
}

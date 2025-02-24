#pragma once
#include "../component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>
#include "ImGuizmo/ImGuizmo.h"
#include <json/json.hpp>

class TransformComponent : public Component
{
public:
    TransformComponent() : position(0.0f),
                           rotation(1.0f, 0.0f, 0.0f, 0.0f), // Identity quaternion
                           scale(1.0f),
                           gizmoOperation(ImGuizmo::TRANSLATE)
    {
    }

    // Core transform properties
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    // Local space vectors
    glm::vec3 forward() const { return glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f)); }
    glm::vec3 right() const { return glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f)); }
    glm::vec3 up() const { return glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f)); }

    // Transform matrices
    glm::mat4 getLocalMatrix() const
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, position);
        transform = transform * glm::toMat4(rotation);
        transform = glm::scale(transform, scale);
        return transform;
    }

    // Utility functions
    void setEulerAngles(const glm::vec3 &eulerDegrees)
    {
        glm::vec3 eulerRadians = glm::radians(eulerDegrees);
        rotation = glm::quat(eulerRadians);
    }

    glm::vec3 getEulerAngles() const
    {
        return glm::degrees(glm::eulerAngles(rotation));
    }

    void lookAt(const glm::vec3 &target)
    {
        glm::vec3 direction = glm::normalize(target - position);
        rotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Gizmo operations
    ImGuizmo::OPERATION getGizmoOperation() const { return gizmoOperation; }
    void setGizmoOperation(ImGuizmo::OPERATION op) { gizmoOperation = op; }

    std::string getTypeName() const override { return "TransformComponent"; }

    // Serialization
    void serialize(nlohmann::json &j) const override
    {
        Component::serialize(j);
        j["position"] = {{"x", position.x}, {"y", position.y}, {"z", position.z}};
        j["rotation"] = {{"w", rotation.w}, {"x", rotation.x}, {"y", rotation.y}, {"z", rotation.z}};
        j["scale"] = {{"x", scale.x}, {"y", scale.y}, {"z", scale.z}};
    }

    void deserialize(const nlohmann::json &j) override
    {
        Component::deserialize(j);

        // Position
        position.x = j["position"]["x"];
        position.y = j["position"]["y"];
        position.z = j["position"]["z"];

        // Rotation - ensure we get a valid quaternion
        rotation.w = j["rotation"]["w"];
        rotation.x = j["rotation"]["x"];
        rotation.y = j["rotation"]["y"];
        rotation.z = j["rotation"]["z"];
        rotation = glm::normalize(rotation); // Ensure unit quaternion

        // Scale
        scale.x = j["scale"]["x"];
        scale.y = j["scale"]["y"];
        scale.z = j["scale"]["z"];
    }

    void OnGUI() override
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Position
            glm::vec3 pos = position;
            if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f))
            {
                position = pos;
            }

            // Rotation (as Euler angles for easier editing)
            glm::vec3 eulerAngles = getEulerAngles();
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerAngles), 1.0f))
            {
                setEulerAngles(eulerAngles);
            }

            // Scale
            glm::vec3 scl = scale;
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scl), 0.1f))
            {
                scale = scl;
            }

            ImGui::Separator();
            ImGui::Text("Gizmo Mode:");

            // Gizmo operation radio buttons
            bool isTranslate = gizmoOperation == ImGuizmo::TRANSLATE;
            bool isRotate = gizmoOperation == ImGuizmo::ROTATE;
            bool isScale = gizmoOperation == ImGuizmo::SCALE;

            if (ImGui::RadioButton("Translate", &isTranslate))
                gizmoOperation = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", &isRotate))
                gizmoOperation = ImGuizmo::ROTATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", &isScale))
                gizmoOperation = ImGuizmo::SCALE;
        }
    }

    void manipulateTransform(const glm::mat4 &view, const glm::mat4 &proj)
    {
        glm::mat4 transform = getLocalMatrix();

        // Manipulate transform with ImGuizmo
        if (ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(proj),
                gizmoOperation,
                ImGuizmo::LOCAL,
                glm::value_ptr(transform)))
        {
            // Decompose the transformed matrix
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::vec3 newPosition;
            glm::quat newRotation;
            glm::vec3 newScale;

            if (glm::decompose(transform, newScale, newRotation, newPosition, skew, perspective))
            {
                position = newPosition;
                rotation = glm::normalize(newRotation); // Ensure unit quaternion
                scale = newScale;
            }
        }
    }

private:
    ImGuizmo::OPERATION gizmoOperation;
};

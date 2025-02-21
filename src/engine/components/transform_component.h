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

class TransformComponent : public Component {
public:
    TransformComponent() = default;
    
    // Core transform properties
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
    
    // Local space vectors
    glm::vec3 forward() const { return glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f)); }
    glm::vec3 right() const { return glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f)); }
    glm::vec3 up() const { return glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f)); }
    
    // Transform matrices
    glm::mat4 getLocalMatrix() const {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
        transform = transform * glm::toMat4(rotation);
        transform = glm::scale(transform, scale);
        return transform;
    }

    // Utility functions
    void setEulerAngles(const glm::vec3& eulerDegrees) {
        rotation = glm::quat(glm::radians(eulerDegrees));
    }

    glm::vec3 getEulerAngles() const {
        return glm::degrees(glm::eulerAngles(rotation));
    }
    
    void lookAt(const glm::vec3& target) {
        glm::vec3 direction = glm::normalize(target - position);
        rotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    std::string getTypeName() const override { return "TransformComponent"; }
    
    void onGUI() override {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Position
            glm::vec3 pos = position;
            if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f)) {
                position = pos;
            }

            // Rotation (as Euler angles for easier editing)
            glm::vec3 eulerAngles = getEulerAngles();
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerAngles), 1.0f)) {
                setEulerAngles(eulerAngles);
            }

            // Scale
            glm::vec3 scl = scale;
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scl), 0.1f)) {
                scale = scl;
            }

            // Gizmo manipulation mode
            if (ImGui::RadioButton("Translate", gizmoOperation == ImGuizmo::TRANSLATE)) {
                gizmoOperation = ImGuizmo::TRANSLATE;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", gizmoOperation == ImGuizmo::ROTATE)) {
                gizmoOperation = ImGuizmo::ROTATE;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", gizmoOperation == ImGuizmo::SCALE)) {
                gizmoOperation = ImGuizmo::SCALE;
            }
        }
    }

    void manipulateTransform(const glm::mat4& view, const glm::mat4& proj) {
        glm::mat4 transform = getLocalMatrix();
        
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
        
        if (ImGuizmo::Manipulate(
            glm::value_ptr(view),
            glm::value_ptr(proj),
            gizmoOperation,
            ImGuizmo::LOCAL,
            glm::value_ptr(transform)
        )) {
            // Decompose the transformed matrix
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::vec3 newPosition;
            glm::quat newRotation;
            glm::vec3 newScale;
            
            if (glm::decompose(transform, newScale, newRotation, newPosition, skew, perspective)) {
                position = newPosition;
                rotation = newRotation;
                scale = newScale;
            }
        }
    }

private:
    static inline ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;
};

/**
 * @file meshrenderer.cpp
 * @brief MeshRenderer component for rendering meshes
 */
#include "meshrenderer.h"
#include "../gameobject.h"
#include "../resourcemanager.h"
#include "imgui.h"

// Note: Most of the MeshRenderer component's functionality is implemented in the header
// since it's primarily getters/setters and the core functionality is handled
// in the render method. This cpp file exists mainly for proper linking.

MeshRenderer::MeshRenderer()
    : mesh(nullptr)
    , color(0.7f, 0.2f, 0.2f)
    , wireframe(false)
{
}

void MeshRenderer::render(Shader& shader) {
    if (!enabled || !mesh) return;

    // Set material properties
    shader.setVec3("objectColor", color);

    // Render mesh
    mesh->Draw();
}

void MeshRenderer::onGUI() {
    if (ImGui::TreeNode("Mesh Renderer")) {
        // Mesh selection
        const char* meshTypes[] = { "None", "Cube", "Sphere" };
        int currentMesh = 0;
        if (mesh) {
            if (mesh == Resources().getMesh("Cube")) currentMesh = 1;
            else if (mesh == Resources().getMesh("Sphere")) currentMesh = 2;
        }

        if (ImGui::Combo("Mesh", &currentMesh, meshTypes, IM_ARRAYSIZE(meshTypes))) {
            switch (currentMesh) {
            case 0: mesh = nullptr; break;
            case 1: mesh = Resources().getMesh("Cube"); break;
            case 2: mesh = Resources().getMesh("Sphere"); break;
            }
        }

        // Color
        ImGui::ColorEdit3("Color", &color.x);

        // Wireframe mode
        ImGui::Checkbox("Wireframe", &wireframe);

        ImGui::TreePop();
    }
}

void MeshRenderer::serialize(json& j) const {
    Component::serialize(j);
    
    // Save mesh type
    std::string meshType = "None";
    if (mesh) {
        if (mesh == Resources().getMesh("Cube")) meshType = "Cube";
        else if (mesh == Resources().getMesh("Sphere")) meshType = "Sphere";
    }
    j["meshType"] = meshType;
    
    // Save color
    j["color"] = { color.r, color.g, color.b };
    j["wireframe"] = wireframe;
}

void MeshRenderer::deserialize(const json& j) {
    Component::deserialize(j);
    
    // Load mesh type
    std::string meshType = j["meshType"].get<std::string>();
    if (meshType == "None") mesh = nullptr;
    else if (meshType == "Cube") mesh = Resources().getMesh("Cube");
    else if (meshType == "Sphere") mesh = Resources().getMesh("Sphere");
    
    // Load color
    auto colorArray = j["color"].get<std::vector<float>>();
    color = glm::vec3(colorArray[0], colorArray[1], colorArray[2]);
    wireframe = j["wireframe"].get<bool>();
}

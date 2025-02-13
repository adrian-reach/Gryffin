#pragma once
#include "../component.h"
#include "../gameobject.h"
#include "../resourcemanager.h"
#include "../../renderer/mesh.h"
#include "../../renderer/shader.h"
#include <imgui.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>

class MeshRenderer : public Component
{
public:
    MeshRenderer() : mesh(nullptr), color(0.7f, 0.2f, 0.2f) {}

    // Component interface
    void render(Shader &shader) override
    {
        if (!mesh)
            return;

        // Set material properties
        shader.setVec3("objectColor", color);

        // Render mesh
        mesh->Draw();
    }

    void onGUI() override
    {
        // ImGui controls for mesh renderer properties
        if (ImGui::ColorEdit3("Color", &color[0]))
        {
            // Color updated through UI
        }

        // Mesh selection
        const char *meshTypes[] = {"None", "Cube", "Sphere"};
        int currentMesh = 0;
        if (mesh)
        {
            // Determine current mesh type
            if (mesh == Resources().getMesh("Cube"))
                currentMesh = 1;
            else if (mesh == Resources().getMesh("Sphere"))
                currentMesh = 2;
        }

        if (ImGui::Combo("Mesh", &currentMesh, meshTypes, IM_ARRAYSIZE(meshTypes)))
        {
            switch (currentMesh)
            {
            case 0: // None
                mesh = nullptr;
                break;
            case 1: // Cube
                mesh = Resources().getMesh("Cube");
                break;
            case 2: // Sphere
                mesh = Resources().getMesh("Sphere");
                break;
            }
        }
    }

    std::string getTypeName() const override
    {
        return "MeshRenderer";
    }

    // Mesh management
    void setMesh(std::shared_ptr<Mesh> newMesh)
    {
        mesh = newMesh;
    }

    std::shared_ptr<Mesh> getMesh() const
    {
        return mesh;
    }

    // Material properties
    void setColor(const glm::vec3 &newColor)
    {
        color = newColor;
    }

    const glm::vec3 &getColor() const
    {
        return color;
    }

private:
    std::shared_ptr<Mesh> mesh;
    glm::vec3 color;
};

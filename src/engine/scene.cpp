/**
 * @file scene.cpp
 * This file contains the implementation for the Scene class, which is responsible
 * for managing game objects and rendering the scene.
 */
#include <algorithm>

#include "components/light.h"
#include "components/meshrenderer.h"
#include "scene.h"

void Scene::render(Shader &shader, GameObject *selectedObject)
{
    // First collect and apply all lights
    std::vector<Light *> lights;
    for (const auto &gameObject : gameObjects)
    {
        if (!gameObject->isActive)
            continue;

        if (auto light = gameObject->getComponent<Light>())
        {
            lights.push_back(light);
        }
    }

    // Apply light properties
    if (!lights.empty())
    {
        auto mainLight = lights[0];
        shader.setVec3("lightPos", mainLight->getOwner()->getPosition());
        shader.setVec3("lightColor", mainLight->getColor() * mainLight->getIntensity());
    }
    else
    {
        // Default light if no lights in scene
        shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        shader.setVec3("lightColor", glm::vec3(1.0f));
    }

    // Render all objects
    for (const auto &gameObject : gameObjects)
    {
        if (!gameObject->isActive)
            continue;

        // Set model matrix from TransformComponent
        shader.setMat4("model", gameObject->getModelMatrix());

        // Render mesh if present
        if (auto meshRenderer = gameObject->getComponent<MeshRenderer>())
        {
            meshRenderer->render(shader);
        }
    }

    // After rendering all objects, handle gizmo manipulation for selected object
    if (selectedObject)
    {
        if (auto transform = selectedObject->getTransform())
        {
            // Get current view/projection matrices
            // TODO: Replace with proper camera system
            glm::mat4 view = glm::lookAt(glm::vec3(0, 5, 10), glm::vec3(0), glm::vec3(0, 1, 0));
            float aspectRatio = ImGui::GetIO().DisplaySize.x / ImGui::GetIO().DisplaySize.y;
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

            // Begin ImGuizmo frame
            ImGuizmo::BeginFrame();
            
            // Set the viewport for ImGuizmo (should match your rendering viewport)
            ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
            
            // Set orthographic mode based on camera
            ImGuizmo::SetOrthographic(false);

            // Manipulate transform
            transform->manipulateTransform(view, proj);
        }
    }
}

void Scene::update(float deltaTime)
{
    for (const auto &gameObject : gameObjects)
    {
        if (gameObject->isActive)
        {
            gameObject->update(deltaTime);
        }
    }
}

void Scene::saveToFile(const std::string &path)
{
    // TODO: Implement scene serialization
}

void Scene::loadFromFile(const std::string &path)
{
    // TODO: Implement scene deserialization
}

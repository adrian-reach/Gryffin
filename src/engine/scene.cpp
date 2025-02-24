/**
 * @file scene.cpp
 * This file contains the implementation for the Scene class, which is responsible
 * for managing game objects and rendering the scene.
 */
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "components/light.h"
#include "components/meshrenderer.h"
#include "components/script_component.h"
#include "scene.h"
#include "../helpers/logging.h"
#include <json/json.hpp>

void Scene::render(Shader &shader, GameObject *selectedObject)
{
    // First collect and apply all lights
    std::vector<Light *> lights;
    for (const auto &gameObject : gameObjects)
    {
        if (!gameObject || !gameObject->isActive)
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
        if (!gameObject || !gameObject->isActive)
            continue;

        // Set model matrix from TransformComponent
        shader.setMat4("model", gameObject->getModelMatrix());

        // Render all components
        for (const auto &component : gameObject->getAllComponents())
        {
            if (component && component->isEnabled())
            {
                component->Render(shader);
            }
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
        if (gameObject && gameObject->isActive)
        {
            // Update all components
            gameObject->update(deltaTime);

            // If the object has a script, call OnUpdate
            // TODO: What happens if the script is disabled, or if there are multiple scripts?
            if (auto script = gameObject->getComponent<ScriptComponent>())
            {
                script->Update(deltaTime);
            }
        }
    }
}

void Scene::clearScene()
{
    for (auto &gameObject : gameObjects)
    {
        if (gameObject)
        {
            gameObject->clearComponents();
        }
    }
    gameObjects.clear();
}

void Scene::saveToFile(const std::string &path)
{
    try
    {
        // Create JSON object
        json j;
        serialize(j);

        // Open file for writing
        std::ofstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file for writing: " + path);
        }

        // Write JSON with pretty printing (4 spaces indent)
        file << std::setw(4) << j << std::endl;

        if (file.fail())
        {
            throw std::runtime_error("Failed to write to file: " + path);
        }

        LOG_INFO("Scene saved successfully to {}", path);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed to save scene: {}", e.what());
    }
}

bool Scene::loadFromFile(const std::string &path)
{
    try
    {
        // Open file for reading
        std::ifstream file(path);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open file for reading: {}", path);
            return false;
        }

        // Parse JSON
        json j;
        file >> j;

        if (file.fail())
        {
            LOG_ERROR("Failed to read from file: {}", path);
            return false;
        }

        // Store current state in case we need to restore it
        json currentState;
        serialize(currentState);

        try
        {
            clearScene(); // Clear existing scene properly
            deserialize(j);
            LOG_INFO("Scene loaded successfully from {}", path);
            return true;
        }
        catch (const std::exception &e)
        {
            // Get the location of the error in the file, and print it to the log
            size_t byteLocation = file.tellg();
            size_t line = 1;
            size_t column = 1;
            file.seekg(0);

            for (size_t i = 0; i < byteLocation; ++i)
            {
                char c;
                file.get(c);
                if (c == '\n')
                {
                    ++line;
                    column = 1;
                }
                else
                {
                    ++column;
                }
            }

            LOG_ERROR("Failed to deserialize scene at line {} column {}: {}", line, column, e.what());

            // LOG_ERROR("Failed to deserialize scene, restoring previous state: {}", e.what());
            clearScene(); // Clear any partial state
            deserialize(currentState);
            return false;
        }
    }
    catch (const json::parse_error &e)
    {
        LOG_ERROR("Failed to parse scene file {}: {}", path, e.what());
        return false;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed to load scene from {}: {}", path, e.what());
        return false;
    }
}

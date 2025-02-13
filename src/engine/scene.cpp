#include "scene.h"
#include "components/light.h"
#include "components/meshrenderer.h"
#include <algorithm>

void Scene::render(Shader &shader)
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
        shader.setVec3("lightPos", mainLight->getPosition());
        shader.setVec3("lightColor", mainLight->getColor() * mainLight->getIntensity());
    }
    else
    {
        // Default light if no lights in scene
        shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        shader.setVec3("lightColor", glm::vec3(1.0f));
    }

    // First render static objects
    for (const auto &gameObject : gameObjects)
    {
        if (gameObject->isActive && gameObject->isStatic)
        {
            // Set model matrix
            shader.setMat4("model", gameObject->transform.getModelMatrix());

            // Render components
            gameObject->render(shader);
        }
    }

    // Then render dynamic objects
    for (const auto &gameObject : gameObjects)
    {
        if (gameObject->isActive && !gameObject->isStatic)
        {
            // Set model matrix
            shader.setMat4("model", gameObject->transform.getModelMatrix());

            // Render components
            gameObject->render(shader);
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

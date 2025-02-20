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
        shader.setVec3("lightPos", mainLight->getPosition());
        shader.setVec3("lightColor", mainLight->getColor() * mainLight->getIntensity());
    }
    else
    {
        // Default light if no lights in scene
        shader.setVec3("lightPos", glm::vec3(2.0f, 2.0f, 2.0f));
        shader.setVec3("lightColor", glm::vec3(1.0f));
    }

    // Render function to handle both normal and outline passes
    auto renderObjects = [&](bool isStatic) {
        for (const auto &gameObject : gameObjects)
        {
            if (!gameObject->isActive || gameObject->isStatic != isStatic)
                continue;

            // Set model matrix
            shader.setMat4("model", gameObject->transform.getModelMatrix());

            // If this is the selected object and we have an outline shader active,
            // we need to render it with a scale factor
            if (gameObject.get() == selectedObject && shader.getProgramID() == Resources().getShader("outline")->getProgramID())
            {
                shader.setFloat("scaleFactor", 1.05f); // Slightly larger for outline
            }
            else
            {
                shader.setFloat("scaleFactor", 1.0f); // Normal size for regular rendering
            }

            // Render components
            gameObject->render(shader);
        }
    };

    // First render static objects
    renderObjects(true);

    // Then render dynamic objects
    renderObjects(false);

    // If we have a selected object and we're using the main shader,
    // render the outline pass
    if (selectedObject && shader.getProgramID() == Resources().getShader("basic")->getProgramID())
    {
        // Switch to outline shader
        auto outlineShader = Resources().getShader("outline");
        if (outlineShader)
        {
            // First pass - render the object normally with stencil write
            shader.use();
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            
            // Set model matrix for selected object
            auto modelMatrix = selectedObject->transform.getModelMatrix();
            shader.setMat4("model", modelMatrix);
            
            // Render the selected object to the stencil buffer
            if (auto renderer = selectedObject->getComponent<MeshRenderer>())
            {
                renderer->render(shader);
            }

            // Second pass - render the scaled outline
            outlineShader->use();
            
            // Copy view/projection matrices from main shader
            outlineShader->setMat4("projection", shader.getUniformMat4("projection"));
            outlineShader->setMat4("view", shader.getUniformMat4("view"));
            
            // Set outline color
            outlineShader->setVec3("outlineColor", glm::vec3(1.0f, 0.5f, 0.0f)); // Orange outline
            
            // Only render where the stencil buffer is not 1
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00); // Disable writing to stencil buffer
            glDisable(GL_DEPTH_TEST);
            
            // Set model matrix and scale factor for outline
            outlineShader->setMat4("model", modelMatrix);
            outlineShader->setFloat("scaleFactor", 1.05f);
            
            // Render the outline
            if (auto renderer = selectedObject->getComponent<MeshRenderer>())
            {
                renderer->render(*outlineShader);
            }
            
            // Restore state
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glEnable(GL_DEPTH_TEST);
            
            // Switch back to main shader
            shader.use();
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

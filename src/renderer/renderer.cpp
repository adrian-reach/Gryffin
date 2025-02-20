/**
 * @file renderer.cpp
 * @brief Renderer class for handling rendering operations
 */

#include <glm/gtc/matrix_transform.hpp>

#include "renderer.h"
#include "../engine/resourcemanager.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::initialize(int width, int height)
{
    viewportWidth = width;
    viewportHeight = height;

    // Create and initialize the shader
    auto basicShader = std::make_shared<Shader>("src/shaders/basic.vert", "src/shaders/basic.frag");
    Resources().addShader("basic", basicShader);
    shader = basicShader;

    // Create and initialize the outline shader
    auto outlineShaderPtr = std::make_shared<Shader>("src/shaders/outline.vert", "src/shaders/outline.frag");
    Resources().addShader("outline", outlineShaderPtr);
    outlineShader = outlineShaderPtr;

    // Initialize camera
    camera.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.setRotation(0.0f, 0.0f);

    // Create basic meshes and add them to resource manager
    auto cubeMesh = std::make_shared<Mesh>(Mesh::CreateCube());
    auto sphereMesh = std::make_shared<Mesh>(Mesh::CreateSphere(1.0f, 32));

    Resources().addMesh("Cube", cubeMesh);
    Resources().addMesh("Sphere", sphereMesh);

    // Keep local references
    cube = cubeMesh;
    sphere = sphereMesh;

    // Set initial viewport
    glViewport(0, 0, width, height);
}

void Renderer::resize(int width, int height)
{
    viewportWidth = width;
    viewportHeight = height;
    glViewport(0, 0, width, height);
}

void Renderer::render(bool useSphere)
{
    if (!shader)
        return;

    shader->use();

    // Set camera matrices
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight),
                                            0.1f, 100.0f);
    shader->setMat4("projection", projection);
    shader->setMat4("view", camera.getViewMatrix());

    // Set camera position for specular lighting
    shader->setVec3("viewPos", camera.getPosition());

    // Set light properties
    shader->setVec3("lightPos", lightPos);
    shader->setVec3("lightColor", lightColor);
    shader->setVec3("objectColor", objectColor);

    // Set model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, objectPosition);

    // Apply rotation for each axis
    model = glm::rotate(model, glm::radians(objectRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(objectRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(objectRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, objectScale);
    shader->setMat4("model", model);

    // Get meshes from resource manager
    auto sphereMesh = Resources().getMesh("Sphere");
    auto cubeMesh = Resources().getMesh("Cube");

    // Render the mesh
    if (useSphere && sphereMesh)
    {
        sphereMesh->Draw();
    }
    else if (!useSphere && cubeMesh)
    {
        cubeMesh->Draw();
    }
}

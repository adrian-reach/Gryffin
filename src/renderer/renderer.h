/**
 * @file renderer.h
 * @brief Renderer class for handling rendering operations
 */

#pragma once
#include <memory>

#include "shader.h"
#include "mesh.h"
#include "camera.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void initialize(int width, int height);
    void resize(int width, int height);

    Camera &getCamera() { return camera; }
    const Camera &getCamera() const { return camera; }

    Shader *getShader() { return shader.get(); }
    const Shader *getShader() const { return shader.get(); }

    // Scene properties
    glm::vec3 lightPos{2.0f, 2.0f, 2.0f};
    glm::vec3 lightColor{1.0f, 1.0f, 1.0f};
    glm::vec3 objectColor{0.7f, 0.2f, 0.2f};
    glm::vec3 objectPosition{0.0f, 0.0f, 0.0f};
    glm::vec3 objectRotation{0.0f, 0.0f, 0.0f};
    glm::vec3 objectScale{1.0f, 1.0f, 1.0f};

    // Outline properties
    glm::vec3 outlineColor{1.0f, 0.5f, 0.0f};  // Orange outline by default
    float outlineScale{1.05f};  // Scale factor for the outline effect

    // Render with specified mesh (true for sphere, false for cube)
    void render(bool useSphere = true);

    // Get viewport dimensions
    int getWidth() const { return viewportWidth; }
    int getHeight() const { return viewportHeight; }

private:
    void setupScene();

    std::shared_ptr<Shader> shader;
    std::shared_ptr<Shader> outlineShader;  // Shader for rendering outlines
    std::shared_ptr<Mesh> cube;
    std::shared_ptr<Mesh> sphere;
    Camera camera;

    int viewportWidth{1280};
    int viewportHeight{720};
};

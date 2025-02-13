/**
 * @file mesh.h
 * @brief Mesh class for handling mesh data
 */
#pragma once
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    ~Mesh();

    // Prevent copying to avoid double deletion of OpenGL resources
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    // Allow moving
    Mesh(Mesh &&other) noexcept;
    Mesh &operator=(Mesh &&other) noexcept;

    void Draw() const;
    static Mesh CreateCube();
    static Mesh CreateSphere(float radius, unsigned int segments);

    // Get the vertex array object
    GLuint getVAO() const { return VAO; }

private:
    bool setupMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    void cleanup();

    GLuint VAO{0}, VBO{0}, EBO{0};
    size_t indexCount{0};
};

#include "mesh.h"
#include <glad/glad.h>
#include <stdio.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Helper function to check OpenGL errors
void checkGLError(const char *location)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        fprintf(stderr, "OpenGL error at %s: 0x%x\n", location, error);
    }
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    setupMesh(vertices, indices);
}

Mesh::~Mesh()
{
    cleanup();
}

Mesh::Mesh(Mesh &&other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), indexCount(other.indexCount)
{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.indexCount = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept
{
    if (this != &other)
    {
        cleanup();

        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        indexCount = other.indexCount;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.indexCount = 0;
    }
    return *this;
}

void Mesh::Draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh Mesh::CreateCube()
{
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},

        // Top face
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},

        // Right face
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

        // Left face
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}};

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };

    return Mesh(vertices, indices);
}

Mesh Mesh::CreateSphere(float radius, unsigned int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices
    for (unsigned int y = 0; y <= segments; ++y)
    {
        float phi = static_cast<float>(M_PI) * static_cast<float>(y) / static_cast<float>(segments);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (unsigned int x = 0; x <= segments; ++x)
        {
            float theta = 2.0f * static_cast<float>(M_PI) * static_cast<float>(x) / static_cast<float>(segments);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            // Calculate vertex position
            float px = cosTheta * sinPhi;
            float py = cosPhi;
            float pz = sinTheta * sinPhi;

            Vertex vertex;
            vertex.Position = {radius * px, radius * py, radius * pz};
            vertex.Normal = {px, py, pz}; // For a sphere, normal is just the normalized position
            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (unsigned int y = 0; y < segments; ++y)
    {
        for (unsigned int x = 0; x < segments; ++x)
        {
            unsigned int current = y * (segments + 1) + x;
            unsigned int next = current + 1;
            unsigned int bottom = (y + 1) * (segments + 1) + x;
            unsigned int bottomNext = bottom + 1;

            indices.push_back(current);
            indices.push_back(bottom);
            indices.push_back(next);

            indices.push_back(next);
            indices.push_back(bottom);
            indices.push_back(bottomNext);
        }
    }

    return Mesh(vertices, indices);
}

bool Mesh::setupMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    indexCount = indices.size();

    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    checkGLError("glGenVertexArrays");

    glGenBuffers(1, &VBO);
    checkGLError("glGenBuffers VBO");

    glGenBuffers(1, &EBO);
    checkGLError("glGenBuffers EBO");

    glBindVertexArray(VAO);
    checkGLError("glBindVertexArray");

    // Load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    checkGLError("glBufferData VBO");

    // Load data into element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    checkGLError("glBufferData EBO");

    // Set vertex attribute pointers
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Position));
    checkGLError("Position attribute");

    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    checkGLError("Normal attribute");

    glBindVertexArray(0);
    return true;
}

void Mesh::cleanup()
{
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0)
    {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}

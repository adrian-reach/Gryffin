#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
    Transform()
        : position(0.0f), rotation(0.0f), scale(1.0f)
    {
    }

    // Direct access to transform properties
    glm::vec3 position;
    glm::vec3 rotation; // Euler angles in degrees
    glm::vec3 scale;

    // Get the model matrix for rendering
    glm::mat4 getModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);

        // Apply transformations in order: Scale -> Rotate -> Translate
        model = glm::translate(model, position);

        // Apply rotation for each axis
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);

        return model;
    }

    // Set transform from a model matrix
    void setFromModelMatrix(const glm::mat4 &matrix)
    {
        // Extract position
        position = glm::vec3(matrix[3]);

        // Extract scale
        scale.x = glm::length(glm::vec3(matrix[0]));
        scale.y = glm::length(glm::vec3(matrix[1]));
        scale.z = glm::length(glm::vec3(matrix[2]));

        // Extract rotation
        glm::mat3 rotMat(
            glm::vec3(matrix[0]) / scale.x,
            glm::vec3(matrix[1]) / scale.y,
            glm::vec3(matrix[2]) / scale.z);

        rotation = glm::degrees(glm::eulerAngles(glm::quat_cast(rotMat)));
    }

    // Utility functions for transformation
    void translate(const glm::vec3 &offset)
    {
        position += offset;
    }

    void rotate(const glm::vec3 &eulerAngles)
    {
        rotation += eulerAngles;
    }

    void scaleBy(const glm::vec3 &factor)
    {
        scale *= factor;
    }

    // Get forward, right, and up vectors
    glm::vec3 forward() const
    {
        glm::mat4 rotMatrix = glm::mat4(1.0f);
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        return glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    }

    glm::vec3 right() const
    {
        glm::mat4 rotMatrix = glm::mat4(1.0f);
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        return glm::normalize(glm::vec3(rotMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
    }

    glm::vec3 up() const
    {
        glm::mat4 rotMatrix = glm::mat4(1.0f);
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotMatrix = glm::rotate(rotMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        return glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    }
};

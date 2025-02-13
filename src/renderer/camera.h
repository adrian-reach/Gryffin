#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 100.0f)
        : position(0.0f, 0.0f, 3.0f), front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
    {
        updateVectors();
    }

    void setPosition(const glm::vec3 &pos) { position = pos; }
    void setFront(const glm::vec3 &f)
    {
        front = glm::normalize(f);
        updateVectors();
    }
    void setAspectRatio(float ratio) { aspectRatio = ratio; }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix() const
    {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    const glm::vec3 &getPosition() const { return position; }
    const glm::vec3 &getFront() const { return front; }
    const glm::vec3 &getUp() const { return up; }
    const glm::vec3 &getRight() const { return right; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

    void setRotation(float newYaw, float newPitch)
    {
        yaw = newYaw;
        pitch = glm::clamp(newPitch, -89.0f, 89.0f);
        updateVectors();
    }

    void moveForward(float distance) { position += front * distance; }
    void moveRight(float distance) { position += right * distance; }
    void moveUp(float distance) { position += up * distance; }

    void rotate(float yaw, float pitch)
    {
        this->yaw += yaw;
        this->pitch = glm::clamp(this->pitch + pitch, -89.0f, 89.0f);
        updateVectors();
    }

private:
    void updateVectors()
    {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, front));
    }

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
};

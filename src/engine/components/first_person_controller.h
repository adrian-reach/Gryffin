#pragma once
#include "../component.h"
#include "transform_component.h"
#include <SDL2/SDL.h>

class FirstPersonController : public Component {
public:
    FirstPersonController() = default;
    
    // Movement settings
    float moveSpeed = 5.0f;
    float sprintMultiplier = 2.0f;
    float jumpForce = 5.0f;
    float mouseSensitivity = 0.2f;
    
    // Camera constraints
    float minPitch = -89.0f;
    float maxPitch = 89.0f;
    float currentPitch = 0.0f;
    float currentYaw = 0.0f;
    
    void start() override {
        transform = getOwner()->getComponent<TransformComponent>();
        SDL_SetRelativeMouseMode(SDL_TRUE);  // Lock cursor for FPS control
    }
    
    void update(float deltaTime) override {
        if (!transform) return;
        
        handleMouseLook();
        handleMovement(deltaTime);
    }
    
    std::string getTypeName() const override { return "FirstPersonController"; }
    
    void onGUI() override {
        // TODO: ImGui interface for controller settings
    }

private:
    TransformComponent* transform = nullptr;
    bool isSprinting = false;
    
    void handleMouseLook() {
        int mouseX, mouseY;
        SDL_GetRelativeMouseState(&mouseX, &mouseY);
        
        // Update yaw and pitch
        currentYaw += mouseX * mouseSensitivity;
        currentPitch = glm::clamp(currentPitch - mouseY * mouseSensitivity, minPitch, maxPitch);
        
        // Convert Euler angles to quaternion
        glm::quat pitch = glm::angleAxis(glm::radians(currentPitch), glm::vec3(1, 0, 0));
        glm::quat yaw = glm::angleAxis(glm::radians(currentYaw), glm::vec3(0, 1, 0));
        transform->rotation = yaw * pitch;
    }
    
    void handleMovement(float deltaTime) {
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
        glm::vec3 moveDir(0.0f);
        
        // WASD movement
        if (keystate[SDL_SCANCODE_W]) moveDir += transform->forward();
        if (keystate[SDL_SCANCODE_S]) moveDir -= transform->forward();
        if (keystate[SDL_SCANCODE_A]) moveDir -= transform->right();
        if (keystate[SDL_SCANCODE_D]) moveDir += transform->right();
        
        // Sprint
        isSprinting = keystate[SDL_SCANCODE_LSHIFT];
        float currentSpeed = moveSpeed * (isSprinting ? sprintMultiplier : 1.0f);
        
        // Normalize and apply movement
        if (glm::length(moveDir) > 0) {
            moveDir = glm::normalize(moveDir);
            transform->position += moveDir * currentSpeed * deltaTime;
        }
    }
};

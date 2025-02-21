#pragma once
#include "../component.h"
#include "transform_component.h"
#include <functional>

enum class CollisionShape {
    Box,
    Sphere,
    Capsule
};

struct Collision {
    GameObject* other;
    glm::vec3 point;
    glm::vec3 normal;
    float penetration;
};

class ColliderComponent : public Component {
public:
    ColliderComponent() = default;
    
    // Collision properties
    CollisionShape shape = CollisionShape::Box;
    glm::vec3 size{1.0f};  // For box: dimensions, For sphere: radius in x, For capsule: radius in x, height in y
    bool isTrigger = false;
    bool isStatic = false;
    
    // Collision events
    std::function<void(const Collision&)> onCollisionEnter;
    std::function<void(const Collision&)> onCollisionStay;
    std::function<void(const Collision&)> onCollisionExit;
    
    void start() override {
        transform = getOwner()->getComponent<TransformComponent>();
    }
    
    bool checkCollision(ColliderComponent* other, Collision& outCollision) {
        if (!transform || !other || !other->transform) return false;
        
        // Basic sphere collision check for demonstration
        if (shape == CollisionShape::Sphere && other->shape == CollisionShape::Sphere) {
            float combinedRadius = size.x + other->size.x;
            glm::vec3 direction = other->transform->position - transform->position;
            float distance = glm::length(direction);
            
            if (distance < combinedRadius) {
                outCollision.other = other->getOwner();
                outCollision.normal = glm::normalize(direction);
                outCollision.point = transform->position + outCollision.normal * size.x;
                outCollision.penetration = combinedRadius - distance;
                return true;
            }
        }
        
        // TODO: Implement other collision shapes
        return false;
    }
    
    std::string getTypeName() const override { return "ColliderComponent"; }
    
    void onGUI() override {
        // TODO: ImGui interface for collider properties
    }
    
private:
    TransformComponent* transform = nullptr;
};

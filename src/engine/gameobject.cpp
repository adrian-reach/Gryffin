#include "gameobject.h"
#include "components/light.h"
#include "components/meshrenderer.h"

std::atomic<uint64_t> GameObject::nextId{0};

void GameObject::deserialize(const json& j) {
    // Clear existing components except transform
    for (auto it = components.begin(); it != components.end();) {
        if ((*it)->getTypeName() != "TransformComponent") {
            it = components.erase(it);
        } else {
            ++it;
        }
    }

    // Restore basic properties
    j.at("name").get_to(name);
    j.at("isStatic").get_to(isStatic);
    j.at("isActive").get_to(isActive);

    // Deserialize components
    for (const auto& componentJson : j.at("components")) {
        std::string type = componentJson.at("type").get<std::string>();
        
        // Create component based on type
        if (type == "TransformComponent") {
            if (auto transform = getComponent<TransformComponent>()) {
                transform->deserialize(componentJson);
            }
        }
        else if (type == "Light") {
            auto light = addComponent<Light>();
            light->deserialize(componentJson);
        }
        else if (type == "MeshRenderer") {
            auto renderer = addComponent<MeshRenderer>();
            renderer->deserialize(componentJson);
        }
    }
}

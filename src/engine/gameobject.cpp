#include "gameobject.h"
#include "components/meshrenderer.h"
#include "components/light.h"

std::atomic<uint64_t> GameObject::nextId(1);

// void GameObject::deserialize(const json& j) {
//     id = j["id"];
//     name = j["name"];
//     isStatic = j["isStatic"];
//     isActive = j["isActive"];

//     // Clear existing components
//     clearComponents();

//     // Deserialize components
//     const auto& componentsArray = j["components"];
//     for (const auto& componentJson : componentsArray) {
//         std::string typeName = componentJson["type"];
        
//         // Create the appropriate component type
//         std::unique_ptr<Component> component;
//         if (typeName == "TransformComponent") {
//             component = std::make_unique<TransformComponent>();
//             transformComponent = static_cast<TransformComponent*>(component.get());
//         }
//         else if (typeName == "MeshRenderer") {
//             component = std::make_unique<MeshRenderer>();
//         }
//         else if (typeName == "Light") {
//             component = std::make_unique<Light>();
//         }
        
//         if (component) {
//             component->setOwner(this);
//             component->deserialize(componentJson);
//             components.push_back(std::move(component));
//         }
//     }

//     // Ensure we have a transform component
//     if (!transformComponent) {
//         transformComponent = addComponent<TransformComponent>();
//     }
// }

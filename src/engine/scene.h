#pragma once
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include "gameobject.h"
#include "components/light.h"
#include "components/meshrenderer.h"
#include "../renderer/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include "ImGuizmo/ImGuizmo.h"
#include <fstream>
#include "serialization.h"
#include <json/json.hpp> // Assuming you have a json library

class Scene : public ISerializable {
public:
    Scene(const std::string& name = "New Scene") : name(name) {}

    GameObject* createGameObject(const std::string& name = "GameObject") {
        auto gameObject = std::make_unique<GameObject>(name);
        auto ptr = gameObject.get();
        gameObjects.push_back(std::move(gameObject));
        return ptr;
    }

    void removeGameObject(GameObject* gameObject) {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
            [gameObject](const std::unique_ptr<GameObject>& obj) {
                return obj.get() == gameObject;
            });

        if (it != gameObjects.end()) {
            gameObjects.erase(it);
        }
    }

    const std::vector<std::unique_ptr<GameObject>>& getAllGameObjects() const {
        return gameObjects;
    }

    void render(Shader& shader, GameObject* selectedObject = nullptr);
    void update(float deltaTime);

    // Serialization
    void serialize(json& j) const override {
        j["name"] = name;
        
        // Serialize all game objects
        json objectsArray = json::array();
        for (const auto& gameObject : gameObjects) {
            json objectJson;
            gameObject->serialize(objectJson);
            objectsArray.push_back(objectJson);
        }
        j["gameObjects"] = objectsArray;
    }

    void deserialize(const json& j) override {
        // Clear existing objects
        gameObjects.clear();
        
        // Restore scene name
        j.at("name").get_to(name);
        
        // Deserialize game objects
        for (const auto& objectJson : j.at("gameObjects")) {
            auto gameObject = std::make_unique<GameObject>();
            gameObject->deserialize(objectJson);
            gameObjects.push_back(std::move(gameObject));
        }
    }

    // File operations
    void saveToFile(const std::string& path);
    void loadFromFile(const std::string& path);

    GameObject* findGameObjectById(uint64_t id) {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
            [id](const std::unique_ptr<GameObject>& obj) {
                return obj->id == id;
            });

        return it != gameObjects.end() ? it->get() : nullptr;
    }

    GameObject* findGameObjectByName(const std::string& name) {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
            [&name](const std::unique_ptr<GameObject>& obj) {
                return obj->name == name;
            });

        return it != gameObjects.end() ? it->get() : nullptr;
    }

private:
    std::string name;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
};

#pragma once
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include "../helpers/logging.h"
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
    Scene(const std::string& name = "New Scene") : name(name), isPlaying(false) {}
    ~Scene() {
        clearScene();
    }

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
            (*it)->clearComponents();  // Clear components first
            gameObjects.erase(it);
        }
    }

    void clearScene();  // Definition moved to cpp file

    const std::vector<std::unique_ptr<GameObject>>& getAllGameObjects() const {
        return gameObjects;
    }

    void render(Shader& shader, GameObject* selectedObject = nullptr);
    void update(float deltaTime);

    // Serialization
    void serialize(json& j) const override {
        j["name"] = name;
        
        // Serialize all game objects
        j["gameObjects"] = json::array();
        for (const auto& gameObject : gameObjects) {
            if (gameObject) {  // Check for null pointers
                json objectJson;
                gameObject->serialize(objectJson);
                j["gameObjects"].push_back(objectJson);
            }
        }
    }

    void deserialize(const json& j) override {
        clearScene();  // Clear existing objects properly
        
        // Restore scene name
        name = j.at("name").get<std::string>();
        
        // Deserialize game objects
        const auto& objectsArray = j.at("gameObjects");
        for (const auto& objectJson : objectsArray) {
            auto gameObject = std::make_unique<GameObject>();
            gameObject->deserialize(objectJson);
            gameObjects.push_back(std::move(gameObject));
        }
    }

    // File operations
    void saveToFile(const std::string& path);
    bool loadFromFile(const std::string& path);

    GameObject* findGameObjectById(uint64_t id) {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
            [id](const std::unique_ptr<GameObject>& obj) {
                return obj && obj->id == id;  // Check for null
            });

        return it != gameObjects.end() ? it->get() : nullptr;
    }

    GameObject* findGameObjectByName(const std::string& name) {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
            [&name](const std::unique_ptr<GameObject>& obj) {
                return obj && obj->name == name;  // Check for null
            });

        return it != gameObjects.end() ? it->get() : nullptr;
    }

    void setPlayMode(bool playing) { isPlaying = playing; }
    bool getPlayMode() const { return isPlaying; }

private:
    std::string name;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    bool isPlaying;
};

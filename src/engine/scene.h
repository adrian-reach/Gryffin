/**
 * @file scene.h
 * @brief Scene class for managing game objects and rendering the scene
 */

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

class Scene
{
public:
    Scene(const std::string &name = "New Scene") : name(name) {}

    GameObject *createGameObject(const std::string &name = "GameObject")
    {
        auto gameObject = std::make_unique<GameObject>(name);
        auto ptr = gameObject.get();
        gameObjects.push_back(std::move(gameObject));
        return ptr;
    }

    void removeGameObject(GameObject *gameObject)
    {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                               [gameObject](const std::unique_ptr<GameObject> &obj)
                               {
                                   return obj.get() == gameObject;
                               });

        if (it != gameObjects.end())
        {
            gameObjects.erase(it);
        }
    }

    const std::vector<std::unique_ptr<GameObject>> &getAllGameObjects() const
    {
        return gameObjects;
    }

    void render(Shader &shader, GameObject *selectedObject = nullptr);

    void update(float deltaTime);
    void saveToFile(const std::string &path);
    void loadFromFile(const std::string &path);

    // Access to game objects
    GameObject *findGameObjectById(uint64_t id)
    {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                               [id](const std::unique_ptr<GameObject> &obj)
                               {
                                   return obj->id == id;
                               });

        return it != gameObjects.end() ? it->get() : nullptr;
    }

    GameObject *findGameObjectByName(const std::string &name)
    {
        auto it = std::find_if(gameObjects.begin(), gameObjects.end(),
                               [&name](const std::unique_ptr<GameObject> &obj)
                               {
                                   return obj->name == name;
                               });

        return it != gameObjects.end() ? it->get() : nullptr;
    }

private:
    std::string name;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
};

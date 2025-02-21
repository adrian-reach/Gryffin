/**
 * @file scene.h
 * @brief Scene class for managing game objects and rendering the scene
 */

#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "gameobject.h"
#include "../renderer/shader.h"
#include "components/transform_component.h"

class Scene
{
public:
    Scene(const std::string &sceneName = "New Scene") : name(sceneName) {}

    GameObject *createGameObject(const std::string &name)
    {
        auto gameObject = std::make_unique<GameObject>(name);
        GameObject *ptr = gameObject.get();
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

    void render(Shader& shader, GameObject* selectedObject = nullptr);

    void update(float deltaTime);
    void saveToFile(const std::string &path);
    void loadFromFile(const std::string &path);

    // Access to game objects
    const std::vector<std::unique_ptr<GameObject>> &getAllGameObjects() const
    {
        return gameObjects;
    }

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

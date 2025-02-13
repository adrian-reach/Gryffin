/**
 * @file: resourcemanager.h
 * @brief: ResourceManager class for managing resources like meshes and shaders
 */
#pragma once
#include "../renderer/mesh.h"
#include "../renderer/shader.h"
#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    static ResourceManager &getInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;

    // Mesh management
    std::shared_ptr<Mesh> getMesh(const std::string &name)
    {
        auto it = meshes.find(name);
        if (it != meshes.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void addMesh(const std::string &name, std::shared_ptr<Mesh> mesh)
    {
        meshes[name] = mesh;
    }

    // Create and cache basic primitives
    void initializePrimitives()
    {
        // Create cube mesh
        if (!getMesh("Cube"))
        {
            addMesh("Cube", std::make_shared<Mesh>(Mesh::CreateCube()));
        }

        // Create sphere mesh
        if (!getMesh("Sphere"))
        {
            addMesh("Sphere", std::make_shared<Mesh>(Mesh::CreateSphere(1.0f, 32)));
        }
    }

    // Shader management
    std::shared_ptr<Shader> getShader(const std::string &name)
    {
        auto it = shaders.find(name);
        if (it != shaders.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void addShader(const std::string &name, std::shared_ptr<Shader> shader)
    {
        shaders[name] = shader;
    }

    // Resource cleanup
    void cleanup()
    {
        meshes.clear();
        shaders.clear();
    }

private:
    ResourceManager() {}
    ~ResourceManager() { cleanup(); }

    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
};

// Convenience function to get the resource manager instance
inline ResourceManager &Resources()
{
    return ResourceManager::getInstance();
}

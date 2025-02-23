/**
 * @file meshrenderer.h
 * @brief MeshRenderer component for rendering meshes
 */
#pragma once
#include "../component.h"
#include "../../renderer/mesh.h"
#include "../resourcemanager.h"
#include <glm/glm.hpp>
#include <memory>

class MeshRenderer : public Component {
public:
    MeshRenderer();
    virtual ~MeshRenderer() = default;

    // Core functionality
    virtual void render(Shader& shader) override;
    virtual void onGUI() override;

    // Mesh management
    void setMesh(std::shared_ptr<Mesh> newMesh) { mesh = newMesh; }
    std::shared_ptr<Mesh> getMesh() const { return mesh; }

    // Material properties
    void setColor(const glm::vec3& newColor) { color = newColor; }
    const glm::vec3& getColor() const { return color; }

    // Serialization
    virtual void serialize(json& j) const override;
    virtual void deserialize(const json& j) override;

    virtual std::string getTypeName() const override {
        return "MeshRenderer";
    }

private:
    std::shared_ptr<Mesh> mesh;
    glm::vec3 color;
    bool wireframe;
};

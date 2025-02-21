/**
 * @file editor.h
 * @brief Editor class for the engine
 */

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "scene.h"
#include "gameobject.h"
#include "../version.h"
#include "components/meshrenderer.h"
#include "components/light.h"
#include "components/transform_component.h"

#include <imgui.h>
#include <imgui_stdlib.h>

class Editor
{
public:
    Editor() : activeScene(nullptr), selectedObject(nullptr), isPlaying(false) {}

    void setActiveScene(Scene *scene)
    {
        activeScene = scene;
        selectedObject = nullptr;
    }

    GameObject* getSelectedObject() const { return selectedObject; }

    void update()
    {
        if (!activeScene)
            return;

        renderMainMenuBar();
        renderSceneHierarchy();
        renderInspector();
        renderToolbar();
    }

private:
    void renderMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene"))
                {
                    // TODO: Implement new scene creation
                }
                if (ImGui::MenuItem("Save Scene"))
                {
                    // TODO: Implement scene saving
                }
                if (ImGui::MenuItem("Load Scene"))
                {
                    // TODO: Implement scene loading
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("GameObject"))
            {
                if (ImGui::MenuItem("Create Empty"))
                {
                    createGameObject("GameObject");
                }
                if (ImGui::BeginMenu("3D Object"))
                {
                    if (ImGui::MenuItem("Cube"))
                    {
                        createCube();
                    }
                    if (ImGui::MenuItem("Sphere"))
                    {
                        createSphere();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Light"))
                {
                    createLight();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About"))
            {
                ImGui::MenuItem(("Version: " + Engine::VERSION).c_str());
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void renderSceneHierarchy()
    {
        ImGui::Begin("Scene Hierarchy");

        if (activeScene)
        {
            for (const auto &gameObject : activeScene->getAllGameObjects())
            {
                renderGameObjectNode(gameObject.get());
            }
        }

        ImGui::End();
    }

    void renderGameObjectNode(GameObject *gameObject)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if (gameObject == selectedObject)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        // Use ID to make tree nodes unique
        ImGui::PushID(static_cast<int>(gameObject->id));
        bool isOpen = ImGui::TreeNodeEx(gameObject->name.c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            selectedObject = gameObject;
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
            {
                if (selectedObject == gameObject)
                {
                    selectedObject = nullptr;
                }
                activeScene->removeGameObject(gameObject);
                ImGui::EndPopup();
                if (isOpen)
                {
                    ImGui::TreePop();
                }
                ImGui::PopID();
                return;
            }
            ImGui::EndPopup();
        }

        if (isOpen)
        {
            // TODO: Render child objects when implementing hierarchy
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void renderInspector()
    {
        if (ImGui::Begin("Inspector"))
        {
            if (selectedObject)
            {
                // Object header
                ImGui::Text("Selected: %s", selectedObject->name.c_str());
                ImGui::Separator();

                // Transform component
                if (auto transform = selectedObject->getTransform())
                {
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        // Position
                        glm::vec3 position = transform->position;
                        if (ImGui::DragFloat3("Position", &position[0], 0.1f))
                        {
                            transform->position = position;
                        }

                        // Rotation (as Euler angles)
                        glm::vec3 rotation = transform->getEulerAngles();
                        if (ImGui::DragFloat3("Rotation", &rotation[0], 1.0f))
                        {
                            transform->setEulerAngles(rotation);
                        }

                        // Scale
                        glm::vec3 scale = transform->scale;
                        if (ImGui::DragFloat3("Scale", &scale[0], 0.1f))
                        {
                            transform->scale = scale;
                        }
                    }
                }

                // Render GUI for all components
                for (const auto& component : selectedObject->getAllComponents())
                {
                    ImGui::PushID(component.get());
                    if (ImGui::CollapsingHeader(component->getTypeName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        component->onGUI();
                    }
                    ImGui::PopID();
                }

                // Name field
                std::string name = selectedObject->name;
                if (ImGui::InputText("Name", &name))
                {
                    selectedObject->name = name;
                }

                // Active toggle
                bool isActive = selectedObject->isActive;
                if (ImGui::Checkbox("Active", &isActive))
                {
                    selectedObject->isActive = isActive;
                }

                // Static toggle
                bool isStatic = selectedObject->isStatic;
                if (ImGui::Checkbox("Static", &isStatic))
                {
                    selectedObject->isStatic = isStatic;
                }

                // Components
                ImGui::Separator();
                if (ImGui::Button("Add Component"))
                {
                    ImGui::OpenPopup("AddComponentMenu");
                }

                if (ImGui::BeginPopup("AddComponentMenu"))
                {
                    if (ImGui::MenuItem("Mesh Renderer"))
                    {
                        selectedObject->addComponent<MeshRenderer>();
                    }
                    if (ImGui::MenuItem("Light"))
                    {
                        selectedObject->addComponent<Light>();
                    }
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::End();
    }

    void renderToolbar()
    {
        ImGui::Begin("Toolbar");

        if (ImGui::Button(isPlaying ? "Stop" : "Play"))
        {
            isPlaying = !isPlaying;
        }

        ImGui::SameLine();

        if (ImGui::Button("Pause"))
        {
            // TODO: Implement pause functionality
        }

        ImGui::End();
    }

    void createGameObject(const std::string &name)
    {
        if (activeScene)
        {
            selectedObject = activeScene->createGameObject(name);
        }
    }

    void createCube()
    {
        if (activeScene)
        {
            auto obj = activeScene->createGameObject("Cube");
            auto renderer = obj->addComponent<MeshRenderer>();
            renderer->setMesh(Resources().getMesh("Cube"));
            selectedObject = obj;
        }
    }

    void createSphere()
    {
        if (activeScene)
        {
            auto obj = activeScene->createGameObject("Sphere");
            auto renderer = obj->addComponent<MeshRenderer>();
            renderer->setMesh(Resources().getMesh("Sphere"));
            selectedObject = obj;
        }
    }

    void createLight()
    {
        if (activeScene)
        {
            auto obj = activeScene->createGameObject("Light");
            obj->addComponent<Light>();
            selectedObject = obj;
        }
    }

    Scene *activeScene;
    GameObject *selectedObject;
    bool isPlaying;
};

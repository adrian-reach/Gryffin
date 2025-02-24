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
#include "components/light.h"
#include "components/meshrenderer.h"
#include "components/script_component.h"
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

    GameObject *getSelectedObject() const { return selectedObject; }

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
                    setActiveScene(new Scene("New Scene"));
                }
                if (ImGui::MenuItem("Save Scene"))
                {
                    ImGui::OpenPopup("SaveScene");
                }
                if (ImGui::MenuItem("Load Scene"))
                {
                    ImGui::OpenPopup("LoadScene");
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

        // Save Scene Dialog
        if (ImGui::BeginPopupModal("SaveScene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char pathBuffer[256] = "scene.json";
            ImGui::InputText("File Path", pathBuffer, sizeof(pathBuffer));

            if (ImGui::Button("Save"))
            {
                if (activeScene)
                {
                    activeScene->saveToFile(pathBuffer);
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Load Scene Dialog
        if (ImGui::BeginPopupModal("LoadScene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char pathBuffer[256] = "scene.json";
            ImGui::InputText("File Path", pathBuffer, sizeof(pathBuffer));

            if (ImGui::Button("Load"))
            {
                auto newScene = std::make_unique<Scene>();
                newScene->loadFromFile(pathBuffer);
                setActiveScene(newScene.release());
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
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

                        ImGui::Separator();
                        ImGui::Text("Gizmo Mode:");

                        // Gizmo operation radio buttons
                        bool isTranslate = transform->getGizmoOperation() == ImGuizmo::TRANSLATE;
                        bool isRotate = transform->getGizmoOperation() == ImGuizmo::ROTATE;
                        bool isScale = transform->getGizmoOperation() == ImGuizmo::SCALE;

                        if (ImGui::RadioButton("Translate", isTranslate))
                            transform->setGizmoOperation(ImGuizmo::TRANSLATE);
                        ImGui::SameLine();
                        if (ImGui::RadioButton("Rotate", isRotate))
                            transform->setGizmoOperation(ImGuizmo::ROTATE);
                        ImGui::SameLine();
                        if (ImGui::RadioButton("Scale", isScale))
                            transform->setGizmoOperation(ImGuizmo::SCALE);
                    }
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
                    if (ImGui::MenuItem("Script"))
                    {
                        selectedObject->addComponent<ScriptComponent>();
                    }
                    ImGui::EndPopup();
                }

                // Render GUI for all components except Transform
                for (const auto &component : selectedObject->getAllComponents())
                {
                    if (component->getTypeName() != "TransformComponent")
                    { // Skip TransformComponent
                        ImGui::PushID(component.get());
                        if (ImGui::CollapsingHeader(component->getTypeName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            if (auto *scriptComp = dynamic_cast<ScriptComponent *>(component.get()))
                            {
                                // Script component specific GUI
                                std::string path = scriptComp->scriptPath;
                                if (ImGui::InputText("Script Path", &path))
                                {
                                    scriptComp->scriptPath = path;
                                }

                                ImGui::SameLine();
                                if (ImGui::Button("Browse##Script"))
                                {
                                    ImGui::OpenPopup("ScriptBrowser");
                                }

                                // Script browser popup
                                if (ImGui::BeginPopupModal("ScriptBrowser", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                                {
                                    static char scriptPath[256] = "scripts/";
                                    ImGui::InputText("Path", scriptPath, sizeof(scriptPath));

                                    if (ImGui::Button("Select"))
                                    {
                                        scriptComp->scriptPath = scriptPath;
                                        ImGui::CloseCurrentPopup();
                                    }
                                    ImGui::SameLine();
                                    if (ImGui::Button("Cancel"))
                                    {
                                        ImGui::CloseCurrentPopup();
                                    }
                                    ImGui::EndPopup();
                                }
                            }
                            else
                            {
                                component->OnGUI();
                            }
                        }
                        ImGui::PopID();
                    }
                }
            }
        }
        ImGui::End();
    }

    void renderToolbar()
    {
        ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::SetWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f - ImGui::GetWindowSize().x * 0.5f, 20));

        if (!isPlaying)
        {
            if (ImGui::Button("Play"))
            {
                startPlay();
            }
        }
        else
        {
            if (ImGui::Button("Stop"))
            {
                stopPlay();
            }
        }

        ImGui::End();
    }

    void startPlay()
    {
        if (!activeScene) return;
        
        isPlaying = true;
        LOG_INFO("Starting play mode");
        
        // Initialize all script components
        for (const auto& gameObject : activeScene->getAllGameObjects())
        {
            if (gameObject && gameObject->isActive)
            {
                if (auto script = gameObject->getComponent<ScriptComponent>())
                {
                    script->Start();
                }
            }
        }
    }

    void stopPlay()
    {
        if (!activeScene) return;
        
        isPlaying = false;
        LOG_INFO("Stopping play mode");
        
        // Clean up scripts if needed
        for (const auto& gameObject : activeScene->getAllGameObjects())
        {
            if (gameObject)
            {
                if (auto script = gameObject->getComponent<ScriptComponent>())
                {
                    // Reset any script state here if needed
                }
            }
        }
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

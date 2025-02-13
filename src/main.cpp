#include <string>
#include "errors.h"
#include "renderer/renderer.h"
#include "engine/scene.h"
#include "engine/editor.h"
#include "engine/resourcemanager.h"
#include "engine/components/meshrenderer.h"
#include "engine/components/light.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>
#include <glm/gtc/matrix_transform.hpp>

struct EngineState
{
    bool quit = false;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Scene> activeScene;
    std::unique_ptr<Editor> editor;
    bool mouseCaptured = false;
    int lastMouseX = 0;
    int lastMouseY = 0;
};

EngineState g_state;

void initializeScene()
{
    // Create main scene
    g_state.activeScene = std::make_unique<Scene>("Main Scene");
    g_state.editor->setActiveScene(g_state.activeScene.get());

    // Create a light
    auto lightObj = g_state.activeScene->createGameObject("Main Light");
    auto light = lightObj->addComponent<Light>();
    lightObj->transform.position = glm::vec3(2.0f, 2.0f, 2.0f);
    light->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    light->setIntensity(1.0f);

    // Create initial objects
    auto cubeObj = g_state.activeScene->createGameObject("Cube");
    auto cubeMesh = cubeObj->addComponent<MeshRenderer>();
    cubeMesh->setMesh(Resources().getMesh("Cube"));
    cubeMesh->setColor(glm::vec3(0.7f, 0.2f, 0.2f));
    cubeObj->transform.position = glm::vec3(1.0f, 0.0f, 0.0f);

    auto sphereObj = g_state.activeScene->createGameObject("Sphere");
    auto sphereMesh = sphereObj->addComponent<MeshRenderer>();
    sphereMesh->setMesh(Resources().getMesh("Sphere"));
    sphereMesh->setColor(glm::vec3(0.2f, 0.7f, 0.2f));
    sphereObj->transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    // Request an OpenGL 3.3 context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_Window *window = SDL_CreateWindow("DoomLike Editor",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return ERROR_SDL_WINDOW_CREATE;
    }

    // Create an OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
        return ERROR_SDL_GL_CONTEXT_CREATE;
    }

    // Make the context current
    SDL_GL_MakeCurrent(window, gl_context);

    // Enable VSync
    SDL_GL_SetSwapInterval(1);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return ERROR_OGL_LOAD_FAILED;
    }

    // Print OpenGL info
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Setup initial OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);

    // Initialize engine components
    g_state.renderer = std::make_unique<Renderer>();
    g_state.renderer->initialize(1280, 720);
    g_state.editor = std::make_unique<Editor>();

    // Initialize scene
    initializeScene();

    while (!g_state.quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                g_state.quit = true;
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
            {
                g_state.mouseCaptured = true;
                SDL_GetMouseState(&g_state.lastMouseX, &g_state.lastMouseY);
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
            else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT)
            {
                g_state.mouseCaptured = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            else if (event.type == SDL_MOUSEMOTION && g_state.mouseCaptured)
            {
                auto &camera = g_state.renderer->getCamera();
                const float sensitivity = 0.1f;
                float xoffset = event.motion.xrel * sensitivity;
                float yoffset = -event.motion.yrel * sensitivity;
                camera.rotate(xoffset, yoffset);
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    g_state.renderer->resize(event.window.data1, event.window.data2);
                }
                else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                    if (g_state.mouseCaptured)
                    {
                        g_state.mouseCaptured = false;
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                auto &camera = g_state.renderer->getCamera();
                const float moveSpeed = 0.5f;

                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    camera.moveForward(moveSpeed);
                    break;
                case SDLK_s:
                    camera.moveForward(-moveSpeed);
                    break;
                case SDLK_a:
                    camera.moveRight(-moveSpeed);
                    break;
                case SDLK_d:
                    camera.moveRight(moveSpeed);
                    break;
                case SDLK_q:
                    camera.moveUp(moveSpeed);
                    break;
                case SDLK_e:
                    camera.moveUp(-moveSpeed);
                    break;
                }
            }
        }

        // Clear buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Update editor
        g_state.editor->update();

        // Set up shader for scene rendering
        if (auto shader = g_state.renderer->getShader())
        {
            shader->use();

            // Set camera matrices
            glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                    static_cast<float>(g_state.renderer->getWidth()) / g_state.renderer->getHeight(),
                                                    0.1f, 100.0f);
            shader->setMat4("projection", projection);
            shader->setMat4("view", g_state.renderer->getCamera().getViewMatrix());

            // Set camera position for specular lighting
            shader->setVec3("viewPos", g_state.renderer->getCamera().getPosition());

            // Render scene
            if (g_state.activeScene)
            {
                g_state.activeScene->render(*shader);
            }
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    g_state.activeScene.reset();
    g_state.editor.reset();
    g_state.renderer.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

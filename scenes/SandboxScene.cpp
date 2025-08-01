//
// Created by insberr on 7/12/25.
//

#include "SandboxScene.h"

#include <algorithm>
#include <vector>

#include "../systems/GraphicsSystem.h"
#include "../shaders.h"
// glm
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <set>
#include <thread>
#include <ranges>

#include "../systems/InputSystem.h"
#include "../systems/CameraSystem.h"
#include "../systems/ImGuiSystem.h"
#include "../systems/ParticleData.h"
#include "../systems/ParticlesChunk.h"
#include "../systems/Player.h"
#include "../systems/Shapes.h"
#include "../systems/particle_types/ParticleTypeSystem.h"
#include "../systems/particle_types/ParticleType.h"
// Enable glm vector hashing
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "../App.h"
#include "../systems/gui/GUI.h"


namespace SandboxVars {
    /* Private Variables And Functions */
    GLuint shaderProgram { 0 };

    // GLuint particlesBuffer { 0 };
    GLuint particlesColrosBuffer { 0 };

    // Settings
    // int maxRaySteps { 400 };
    // bool enableOutlines { false };
    // init ParticlesChunk static values
    int simulationStepInterval { 20 };
    float simulationStepDelta { 1.0f };

    // Chunks
    using unordered_map = std::unordered_map<glm::ivec3, ParticlesChunk*>;
    unordered_map particleChunks {};

    std::mutex chunksLock;
    std::jthread chunksThread;

    Player player {};

    // GUI
    int mainMenuButton = 0;
    int settingsMenuButton = 0;
    int quitButton = 0;

    struct IVec3Comparator {
        bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            return a.z < b.z;
        }
    };
}

using namespace SandboxVars;

void handleChunksAroundPlayer() {
    auto cameraChunkPosition = PositionConversion::WorldPositionToChunkPosition(CameraSystem::GetPosition() / particleScale, glm::uvec3(chunkSize));
    // std::cout << "Camera is in chunk " << cameraChunkPosition.x << " " << cameraChunkPosition.y << " " << cameraChunkPosition.z << std::endl;
    const int chunkDistance = 5; // Distance to load chunks around the player

    // Determine the required chunk positions around the camera
    std::set<glm::ivec3, IVec3Comparator> requiredChunkPositions;
    for (int x = -chunkDistance; x <= chunkDistance; ++x) {
        for (int y = 0; y < 2; ++y) {
            for (int z = -chunkDistance; z <= chunkDistance; ++z) {
                auto requiredChunkPosition = cameraChunkPosition + glm::ivec3(x, 0, z);

                // // Enforce y to always be 0. We don't do vertical chunks right now
                requiredChunkPosition.y = y;

                requiredChunkPositions.insert(requiredChunkPosition);
            }
        }
    }

    // Manage chunks in one pass

    if (chunksLock.try_lock()) {
        unordered_map updatedChunks {};

        for (auto& [chunkPosition, chunk] : particleChunks) {
            if (requiredChunkPositions.count(chunkPosition)) {
                // Keep chunk if it's within range
                updatedChunks[chunkPosition] = chunk;
                // Remove from required set
                requiredChunkPositions.erase(chunkPosition);
            } else {
                // Remove chunks outside range
                delete chunk;
            }
        }

        // Add missing chunks
        for (const auto& pos : requiredChunkPositions) {
            printf("Loading chunk at position %i %i %i\n", pos.x, pos.y, pos.z);
            updatedChunks[pos] = new ParticlesChunk(pos, glm::uvec3(chunkSize));
            updatedChunks[pos]->InitGraphics();
        }

        particleChunks = std::move(updatedChunks);
        chunksLock.unlock();
    }
}

void SandboxScene::Init() {
    handleChunksAroundPlayer();

    printf("Loaded %lu chunks.\n", particleChunks.size());

    // todo: multithreading stuff
    chunksThread = std::jthread([](const std::stop_token& stoken) {
        while (true)
        {
            if (stoken.stop_requested()) {
                std::cout << "Stop requested for chunks thread" << std::endl;

                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(simulationStepInterval));

            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            chunksLock.lock();
            for (const auto& chunk : std::views::values(particleChunks)) {
                // std::cout << chunk << std::endl;
                chunk->ProcessNextSimulationStep(particleChunks);
            }
            for (const auto& chunk : std::views::values(particleChunks)) {
                chunk->ProcessPostSimulationStep();
            }
            chunksLock.unlock();
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::chrono::duration<float> duration = end - start;
            simulationStepDelta = duration.count();
        }
    });

    std::cout << chunksThread.get_id() << std::endl;

    // Create Pause Menu Stuff
    const glm::ivec2 windowSizeD2 = Graphics::GetWindowSize() / 2;
    mainMenuButton = GUI::CreateButton(
        windowSizeD2,
        glm::vec2(300.0f, 100.0f),
        std::string("Main Menu")
    );
    settingsMenuButton = GUI::CreateButton(
        windowSizeD2 - glm::ivec2(0, 150),
        glm::vec2(300.0f, 100.0f),
        std::string("Settings")
    );
    quitButton = GUI::CreateButton(
        windowSizeD2 - glm::ivec2(0, 300),
        glm::vec2(300.0f, 100.0f),
        std::string("Quit")
    );
    GUI::DisableElement(mainMenuButton);
    GUI::DisableElement(settingsMenuButton);
    GUI::DisableElement(quitButton);

    // Lock the cursor
    Graphics::LockCursor();
}

void SandboxScene::InitGraphics() {
    player.initGraphics();

    // Load the contents of the shaders
    std::string vertexShaderSource = readShaderFile("shaders/vertex_chunk.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment_chunk.glsl");

    // Make sure they arent empty
    if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
        return;
    }

    // Create the shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) return;

    glCreateBuffers(1, &particlesColrosBuffer);
    auto particleColors = ParticleTypeSystem::GetParticleColorIndexesForShader();
    glNamedBufferStorage(
        particlesColrosBuffer,
        sizeof(NormColor) * particleColors.size(),
        (const void*)particleColors.data(),
        GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, particlesColrosBuffer);

    // ChunkConfig::EnableOutlines = false;

    for (const auto& chunk : std::views::values(particleChunks)) {
        chunk->InitGraphics();
    }
}

void SandboxScene::Update(float dt) {
    if (dt == 0.0f) return;

    // Check window focus
    if (!glfwGetWindowAttrib(Graphics::GetWindow(), GLFW_FOCUSED)) {
        // Unlock the cursor and show it
        Graphics::UnLockCursor();
        ImGuiSystem::EnableImGui();

        GUI::EnableElement(mainMenuButton);
        GUI::EnableElement(settingsMenuButton);
        GUI::EnableElement(quitButton);
    }

    if (InputSystem::IsKeyTriggered(GLFW_KEY_ESCAPE)) {
        if (Graphics::IsCursorLocked()) {
            // Unlock the cursor and show it
            Graphics::UnLockCursor();
            ImGuiSystem::EnableImGui();

            GUI::EnableElement(mainMenuButton);
            GUI::EnableElement(settingsMenuButton);
            GUI::EnableElement(quitButton);
        } else {
            // Lock the cursor and hide it
            Graphics::LockCursor();
            ImGuiSystem::DisableImGui();

            GUI::DisableElement(mainMenuButton);
            GUI::DisableElement(settingsMenuButton);
            GUI::DisableElement(quitButton);
        }
    }

    if (GUI::GetElementById(mainMenuButton).isTriggered) {
        SceneSystem::SwitchActiveScene("MainMenuScene");
    }
    if (GUI::GetElementById(settingsMenuButton).isTriggered) {
        // Do nothing yet
    }
    if (GUI::GetElementById(quitButton).isTriggered) {
        App::Shutdown();
    }

    if (!Graphics::IsCursorLocked()) {
        return;
    }

    // TODO: FIXME: It's not great having to pass this to the player update function
    player.update(dt, particleChunks);

    handleChunksAroundPlayer();
}

void SandboxScene::Render() {
    auto window = Graphics::GetWindow();

    // Set the shader program
    glUseProgram(shaderProgram);

    auto projection = CameraSystem::GetProjection();
    auto view = CameraSystem::GetView();

    // todo: multiply these and send as one value to the GPU
    const GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    const GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    const GLint particleScaleLoc = glGetUniformLocation(shaderProgram, "particleScale");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform1f(particleScaleLoc, particleScale);

    // const GLint enableOutlinesLoc = glGetUniformLocation(shaderProgram, "EnableOutlines");
    // glUniform1ui(enableOutlinesLoc, ChunkConfig::EnableOutlines);

    printf("Rendering %lu chunks\n", particleChunks.size());
    for (const auto& chunk : std::views::values(particleChunks)) {
        chunk->Render(window, shaderProgram, particlesColrosBuffer);
    }

    player.render();

    if (!Graphics::IsCursorLocked()) {
        const glm::ivec2 windowSize = Graphics::GetWindowSize();
        Graphics::Draw2D::DrawRectangle(
            windowSize / 2,
            windowSize,
            glm::vec4(0.2f, 0.2f, 0.2f, 0.7f)
        );
    }

    if (ImGui::Begin("Rendering Controls")) {
        if (ImGui::Button("Reload Chunk Shaders")) {
            // Load the contents of the shaders
            std::string vertexShaderSource = readShaderFile("../shaders/vertex_chunk.glsl");
            std::string fragmentShaderSource = readShaderFile("../shaders/fragment_chunk.glsl");

            // Make sure they arent empty
            if (vertexShaderSource.empty() || fragmentShaderSource.empty()) {
                return;
            }

            // Create the shader program
            shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
            if (shaderProgram == 0) return;
        }
    }
    ImGui::End();

    if (ImGui::Begin("Simulation Controls")) {
        ImGui::SliderInt("Simulation Step Interval", &simulationStepInterval, 0, 500);
        ImGui::SliderFloat("Particle Scale", &particleScale, 0.001f, 5.0f, "%.8f");
        // ImGui::Checkbox("Show Particle Outlines", &ChunkConfig::EnableOutlines);
    }
    ImGui::End();

    if (ImGui::Begin("Simulation Stats")) {
        ImGui::Text("Chunk Size %i", chunkSize);
        ImGui::Text("Simulation Step Delta %f", simulationStepDelta);
        ImGui::Text("Simulation Step Time Calculated %f", simulationStepInterval * simulationStepDelta);
    }
    ImGui::End();
}

void SandboxScene::Exit() {
    GUI::RemoveElement(mainMenuButton);
    GUI::RemoveElement(settingsMenuButton);
    GUI::RemoveElement(quitButton);

    chunksThread.request_stop();
    chunksThread.join();

    for (auto& [chunkPosition, chunk] : particleChunks) {
        if (chunk == nullptr) {
            throw std::invalid_argument("Chunk is null during deconstruction. There should not ever be a null chunk");
            continue;
        }
        delete chunk;
        chunk = nullptr;
    }

    particleChunks.clear();
}

SandboxScene::~SandboxScene() {}

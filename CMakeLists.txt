cmake_minimum_required(VERSION 3.28)
project(archea)

set(CMAKE_CXX_STANDARD 23)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

# Include Imgui
include_directories(include/imgui)
include_directories(include/imgui/backends)

# Add FreeType as subdirectory
add_subdirectory(include/freetype)

# Add stb_image
include_directories(include/stb_image)

# Create EXE
add_executable(archea
        main.cpp
        # Include Imgui
        include/imgui/imconfig.h
        include/imgui/imgui.cpp include/imgui/imgui.h
        include/imgui/imgui_demo.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_internal.h
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imstb_rectpack.h
        include/imgui/imstb_textedit.h
        include/imgui/imstb_truetype.h
        include/imgui/backends/imgui_impl_glfw.cpp
        include/imgui/backends/imgui_impl_glfw.h
        include/imgui/backends/imgui_impl_opengl3.cpp
        include/imgui/backends/imgui_impl_opengl3.h
        include/imgui/backends/imgui_impl_opengl3_loader.h
        # App
        App.cpp App.h
        systems/System.h
        shaders.cpp shaders.h
        systems/InputSystem.cpp
        systems/InputSystem.h
        EngineTimer.cpp
        EngineTimer.h
        systems/CameraSystem.cpp
        systems/CameraSystem.h
        systems/ImGuiSystem.cpp
        systems/ImGuiSystem.h
        systems/particle_types/ParticleType.h
        systems/particle_types/SandParticle_Type.h
        systems/particle_types/ParticleTypeSystem.cpp
        systems/particle_types/ParticleTypeSystem.h
        systems/GraphicsSystem.cpp
        systems/GraphicsSystem.h
        systems/particle_types/DebugParticle_Type.h
        systems/particle_types/ParticleMove.cpp
        systems/particle_types/ParticleMove.h
        systems/particle_types/WaterParticle_Type.h
        systems/ParticleData.cpp
        systems/ParticleData.h
        systems/ParticlesChunk.cpp
        systems/ParticlesChunk.h
        systems/Shapes.h
        systems/Player.cpp
        systems/Player.h
        systems/particle_types/ErrorParticle_Type.h
        systems/particle_types/StoneParticleType.h
        systems/particle_types/SteamParticleType.h
        systems/particle_types/LavaParticleType.h
        systems/SceneSystem.cpp
        systems/SceneSystem.h
        scenes/MainMenuScene.cpp
        scenes/MainMenuScene.h
        scenes/SandboxScene.cpp
        scenes/SandboxScene.h
        systems/gui/GUI.cpp
        systems/gui/GUI.h
)

# Link libraries
target_link_libraries(archea ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw glm::glm freetype)

### Copy shader files when they are changed
# Find all shader files
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shaders/*")
set(COPIED_SHADERS "")
foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    set(SHADER_OUT "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}")
    add_custom_command(
            OUTPUT ${SHADER_OUT}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER_FILE} ${SHADER_OUT}
            DEPENDS ${SHADER_FILE}
            COMMENT "Copying shader ${SHADER_NAME} if changed"
            VERBATIM
    )
    list(APPEND COPIED_SHADERS ${SHADER_OUT})
endforeach()
add_custom_target(copy_shaders DEPENDS ${COPIED_SHADERS})
add_dependencies(archea copy_shaders)

### Copy font files when they are changed
# Find all font files
file(GLOB FONT_FILES "${CMAKE_SOURCE_DIR}/fonts/*")
set(COPIED_FONTS "")
foreach(FONT_FILE ${FONT_FILES})
    get_filename_component(FONT_NAME ${FONT_FILE} NAME)
    set(FONT_OUT "${CMAKE_BINARY_DIR}/fonts/${FONT_NAME}")
    add_custom_command(
            OUTPUT ${FONT_OUT}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_FILE} ${FONT_OUT}
            DEPENDS ${FONT_FILE}
            COMMENT "Copying font ${FONT_NAME} if changed"
            VERBATIM
    )
    list(APPEND COPIED_FONTS ${FONT_OUT})
endforeach()
add_custom_target(copy_fonts DEPENDS ${COPIED_FONTS})
add_dependencies(archea copy_fonts)

### Copy sprite files when they are changed
# Find all sprite files
file(GLOB SPRITE_FILES "${CMAKE_SOURCE_DIR}/sprites/*")
set(COPIED_SPRITES "")
foreach(SPRITE_FILE ${SPRITE_FILES})
    get_filename_component(SPRITE_NAME ${SPRITE_FILE} NAME)
    set(SPRITE_OUT "${CMAKE_BINARY_DIR}/sprites/${SPRITE_NAME}")
    add_custom_command(
            OUTPUT ${SPRITE_OUT}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SPRITE_FILE} ${SPRITE_OUT}
            DEPENDS ${SPRITE_FILE}
            COMMENT "Copying sprite ${SPRITE_NAME} if changed"
            VERBATIM
    )
    list(APPEND COPIED_SPRITES ${SPRITE_OUT})
endforeach()
add_custom_target(copy_sprites DEPENDS ${COPIED_SPRITES})
add_dependencies(archea copy_sprites)


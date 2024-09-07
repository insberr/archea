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

# Copy shaders to build directory
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders") # Source directory to copy from
set(DEST_DIR "${CMAKE_BINARY_DIR}/shaders") # Destination directory to copy to
file(COPY ${SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR}) # Copy the files during configuration
# Copy shaders to build directory during the build process
add_custom_command(
        OUTPUT ${DEST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR} ${DEST_DIR}
        DEPENDS ${SOURCE_DIR}
        COMMENT "Copying files from ${SOURCE_DIR} to ${DEST_DIR}"
)
# Custom target to run the custom command
add_custom_target(copy_shaders ALL DEPENDS ${DEST_DIR})

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
        systems/ParticleSystem.cpp systems/ParticleSystem.h
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
        systems/TestSingletonSystem.cpp
        systems/TestSingletonSystem.h
        systems/GraphicsSystem.cpp
        systems/GraphicsSystem.h
        systems/particle_types/DebugParticle_Type.h
        systems/particle_types/ParticleMove.cpp
        systems/particle_types/ParticleMove.h
        systems/particle_types/WaterParticle_Type.h
)
# Run the copy_shaders commands
add_dependencies(archea copy_shaders)

# Link libraries
target_link_libraries(archea ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw glm::glm)

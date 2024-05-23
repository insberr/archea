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
include_directories(imgui)
include_directories(imgui/backends)

# Copy shaders to build directory
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders") # Source directory to copy from
set(DEST_DIR "${CMAKE_BINARY_DIR}/shaders") # Destination directory to copy to
file(COPY ${SOURCE_DIR} DESTINATION ${DEST_DIR}) # Copy the files during configuration
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
        imgui/imconfig.h
        imgui/imgui.cpp imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_opengl3.h
        imgui/backends/imgui_impl_opengl3_loader.h
        # App
        App.cpp App.h
        systems/System.cpp systems/System.h
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
)
# Run the copy_shaders commands
add_dependencies(archea copy_shaders)

# Link libraries
target_link_libraries(archea ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw glm::glm)

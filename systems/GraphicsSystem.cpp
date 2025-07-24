//
// Created by insberr on 5/27/24.
//

#include "GraphicsSystem.h"

#include <iostream>

#include <ft2build.h>
#include <map>
#include <glm/gtc/type_ptr.hpp>

#include FT_FREETYPE_H

#include "CameraSystem.h"
#include "Shapes.h"
#include "../shaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include <unordered_map>

#include "stb_image.h"

// Private values
namespace Graphics {
    /* System Function Declarations */
    int Setup();
    void Init();
    void PrePollEvents();
    void Update(float dt);
    void Done();
    System AsSystem() {
        return {
            .Setup = Setup,
            .Init = Init,
            .PrePollEvents = PrePollEvents,
            .Update = Update,
            .Done = Done,
        };
    }

    /* Private Variables And Functions */

    // The GLFW window
    GLFWwindow* window { nullptr };

    // Did window resize
    bool didWindowResize { false };

    /* Internal Functions */

    // Callback function for errors
    void errorCallback(int error, const char* description);

    // Some callback for frame buffer size
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
}

namespace Graphics::Draw2D {
    glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
    // This flips things the way they are supposed to be smh
    // glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f);

    namespace RectGL {
        GLuint VAO, VBO, EBO;
        GLuint shaderProgram;
    }

    void setupRectBuffers() {
        glGenVertexArrays(1, &RectGL::VAO);
        glGenBuffers(1, &RectGL::VBO);
        glGenBuffers(1, &RectGL::EBO);

        glBindVertexArray(RectGL::VAO);

        glBindBuffer(GL_ARRAY_BUFFER, RectGL::VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes::Rect::rectVertices), Shapes::Rect::rectVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RectGL::EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Shapes::Rect::rectIndices), Shapes::Rect::rectIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void DrawRectangle(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) {
        glUseProgram(RectGL::shaderProgram);

        // Set uniforms
        glUniform2f(glGetUniformLocation(RectGL::shaderProgram, "u_position"), position.x, position.y);
        glUniform2f(glGetUniformLocation(RectGL::shaderProgram, "u_size"), size.x, size.y);
        const glm::ivec2 windowSize = GetWindowSize();
        glUniform2f(glGetUniformLocation(RectGL::shaderProgram, "u_resolution"), windowSize.x, windowSize.y);
        glUniform4f(glGetUniformLocation(RectGL::shaderProgram, "u_color"), color.r, color.g, color.b, color.a);

        glBindVertexArray(RectGL::VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    namespace TextGL {
        struct Character {
            unsigned int TextureID;  // ID handle of the glyph texture
            glm::ivec2   Size;       // Size of glyph
            glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
            unsigned int Advance;    // Offset to advance to next glyph
        };

        std::map<char, Character> Characters;

        unsigned int VAO, VBO;
        GLuint shaderProgram;

        void setupText() {
            FT_Library ft;
            if (FT_Init_FreeType(&ft))
            {
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
                throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
            }

            FT_Face face;
            if (FT_New_Face(ft, "fonts/Ubuntu-Regular.ttf", 0, &face))
            {
                std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
                throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
            }

            FT_Set_Pixel_Sizes(face, 0, 48);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

            for (unsigned char c = 0; c < 128; c++)
            {
                // load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                Characters.insert(std::pair<char, Character>(c, character));
            }

            FT_Done_Face(face);
            FT_Done_FreeType(ft);


            glGenVertexArrays(1, &TextGL::VAO);
            glGenBuffers(1, &TextGL::VBO);
            glBindVertexArray(TextGL::VAO);
            glBindBuffer(GL_ARRAY_BUFFER, TextGL::VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            // Load the contents of the shaders
            std::string vertexShaderSource = readShaderFile("shaders/font_vertex.glsl");
            std::string fragmentShaderSource = readShaderFile("shaders/font_fragment.glsl");
            // Make sure they arent empty
            if (!vertexShaderSource.empty() && !fragmentShaderSource.empty()) {
                // Create the shader program
                TextGL::shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
            }

            glUseProgram(TextGL::shaderProgram);
            glUniformMatrix4fv(
                glGetUniformLocation(shaderProgram, "projection"),
                1,
                GL_FALSE,
                glm::value_ptr(projection)
            );
        }
    }
    void DrawText(const std::string &text, const glm::vec2 &position, float scale, const glm::vec4 &color) {
        float x = position.x;
        float y = position.y;

        // activate corresponding render state
        glUseProgram(TextGL::shaderProgram);
        glUniform3f(glGetUniformLocation(TextGL::shaderProgram, "textColor"), color.x, color.y, color.z);
        // glUniformMatrix4fv(
        //     glGetUniformLocation(TextGL::shaderProgram, "projection"),
        //     1,
        //     GL_FALSE,
        //     glm::value_ptr(TextGL::projection)
        // );
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(TextGL::VAO);

        // iterate through all characters to find total width for text centering
        unsigned int totalWidth = 0;
        std::string::const_iterator cw;
        for (cw = text.begin(); cw != text.end(); cw++) {
            TextGL::Character ch = TextGL::Characters[*cw];
            totalWidth += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        float totalWidthHalf = totalWidth / 2;

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            TextGL::Character ch = TextGL::Characters[*c];

            float xpos = (x - totalWidthHalf) + ch.Bearing.x * scale;
            float ypos = (y - (ch.Size.y - ch.Bearing.y) * scale) - (48 / 4);

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, TextGL::VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    namespace TextureGL {
        struct Texture {
            GLuint ID;
            unsigned int width, height;
            // texture Format
            unsigned int Internal_Format; // format of texture object
            unsigned int Image_Format; // format of loaded image
            // texture configuration
            unsigned int Wrap_S; // wrapping mode on S axis
            unsigned int Wrap_T; // wrapping mode on T axis
            unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
            unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels
        };

        std::unordered_map<std::string, Texture> textures;
        GLuint VAO, VBO;
        GLuint shaderProgram;
        void Init() {
            float vertices[] = {
                // pos      // tex
                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,

                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f
            };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindVertexArray(VAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            // Shader Program
            // Load the contents of the shaders
            std::string vertexShaderSource = readShaderFile("shaders/vertex_sprite.glsl");
            std::string fragmentShaderSource = readShaderFile("shaders/fragment_sprite.glsl");

            // Make sure they arent empty
            if (!vertexShaderSource.empty() && !fragmentShaderSource.empty()) {
                // Create the shader program
                TextureGL::shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
            }
            glUseProgram(TextureGL::shaderProgram);
            glUniform1i(glGetUniformLocation(TextureGL::shaderProgram, "image"), 0);
            glUniformMatrix4fv(
                glGetUniformLocation(TextureGL::shaderProgram, "projection"),
                1,
                GL_FALSE,
                glm::value_ptr(projection)
            );
        }

        Texture loadTextureFromFile(const std::string &filePath, bool alpha) {
            if (textures.contains(filePath)) {
                return textures[filePath];
            }

            // First, load image using stb
            Texture texture {
                .ID = 0,
                .width = 0,
                .height = 0,
                .Internal_Format = GL_RGB,
                .Image_Format = GL_RGB,
                .Wrap_S = GL_CLAMP_TO_EDGE,
                .Wrap_T = GL_CLAMP_TO_EDGE,
                .Filter_Min = GL_NEAREST,
                .Filter_Max = GL_NEAREST
            };
            // Generate texture
            glGenTextures(1, &texture.ID);
            // Load image into texture
            if (alpha)
            {
                texture.Internal_Format = GL_RGBA;
                texture.Image_Format = GL_RGBA;
            }
            // load image
            int width, height, nrChannels;
            unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
            // now generate texture
            texture.width = width;
            texture.height = height;
            // create Texture
            glBindTexture(GL_TEXTURE_2D, texture.ID);
            glTexImage2D(GL_TEXTURE_2D, 0, texture.Internal_Format, width, height, 0, texture.Image_Format, GL_UNSIGNED_BYTE, data);
            // set Texture wrap and filter modes
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.Wrap_S);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.Wrap_T);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.Filter_Min);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.Filter_Max);
            // unbind texture
            glBindTexture(GL_TEXTURE_2D, 0);
            // and finally free image data
            stbi_image_free(data);
            textures[filePath] = texture;
            return texture;
        }
        void destroyTexture(const std::string& filePath) {}
        void destroyTexture(Texture* texture) {}
    }
    void DrawSprite(
        const std::string& filePath,
        const glm::vec2& position,
        const glm::vec2& size,
        float rotate,
        const glm::vec3& color
    ) {
        TextureGL::Texture texture = TextureGL::loadTextureFromFile(filePath, true);

        // prepare transformations
        glUseProgram(TextureGL::shaderProgram);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

        model = glm::scale(model, glm::vec3(size, 1.0f));

        glUniformMatrix4fv(
            glGetUniformLocation(TextureGL::shaderProgram, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );
        glUniform3f(glGetUniformLocation(TextureGL::shaderProgram, "spriteColor"), color.x, color.y, color.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.ID);

        glBindVertexArray(TextureGL::VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

int Graphics::Setup() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set the GLFW error callback
    glfwSetErrorCallback(errorCallback);

    // Give hints to glfw about version we wish to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window using glfw
    window = glfwCreateWindow(WindowWidth, WindowHeight, "Archea", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make window context current
    glfwMakeContextCurrent(window);

    // Request to disable vsync
    // glfwSwapInterval(0);

    // note: figure out what this does too
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

    return 0;
}

void Graphics::Init() {
    Draw2D::setupRectBuffers();
    // Load the contents of the shaders
    std::string vertexShaderSource = readShaderFile("shaders/vertex_rect.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment_rect.glsl");

    // Make sure they arent empty
    if (!vertexShaderSource.empty() && !fragmentShaderSource.empty()) {
        // Create the shader program
        Draw2D::RectGL::shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    }
    Draw2D::TextGL::setupText();

    Draw2D::TextureGL::Init();
}

void Graphics::PrePollEvents() {
    didWindowResize = false;
}

void Graphics::Update(float dt) {
    glm::ivec2 windowSize = GetWindowSize();
    Draw2D::projection = glm::ortho(0.0f, static_cast<float>(windowSize.x), 0.0f, static_cast<float>(windowSize.y));

    glUseProgram(Draw2D::TextGL::shaderProgram);
    glUniformMatrix4fv(
        glGetUniformLocation(Draw2D::TextGL::shaderProgram, "projection"),
        1,
        GL_FALSE,
        glm::value_ptr(Draw2D::projection)
    );

    glUseProgram(Draw2D::TextureGL::shaderProgram);
    glUniformMatrix4fv(
        glGetUniformLocation(Draw2D::TextureGL::shaderProgram, "projection"),
        1,
        GL_FALSE,
        glm::value_ptr(Draw2D::projection)
    );
}

void Graphics::Done() {
    // Free GLFW window, GLFW, and GLEW if needed

    // Shutdown GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Graphics::GetWindow() {
    return window;
}

glm::ivec2 Graphics::GetWindowSize() {
    glm::ivec2 windowSize;
    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
    return windowSize;
}

bool Graphics::DidWindowResize() {
    return didWindowResize;
}

GLuint Graphics::CreateShaderProgram() {
    return 0; // todo
}

/* Private Internal Functions */
void Graphics::errorCallback(int error, const char *description) {
    std::cerr << "Error: " << description << std::endl;
}

void Graphics::framebufferSizeCallback(GLFWwindow* wind, int width, int height) {
    glViewport(0, 0, width, height);
    CameraSystem::setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    didWindowResize = true;
}

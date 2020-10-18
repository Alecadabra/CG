// Local Headers
#include "4.transformations.h"

// Classes
#include "Texture.h"
#include "Shader.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/filesystem.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void framebuffer_size_callback(GLFWwindow *, int, int);
void processInput(GLFWwindow *);

int main(int argc, char* argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Alec OpenGL",
        nullptr,
        nullptr
    );

    // Check for Valid Context
    if (window == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Create Context
    glfwMakeContextCurrent(window);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return EXIT_FAILURE;
    }

    // Tell OpenGL the size of the rendering window
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Give callback for any window size changes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);    

    // A rectangle using triangles
    float verticies[] = {
        //  positions            colours           texture coords
        //  x,     y,     z,     r,    g,    b,    x,    y
         0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f,  0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indicies[] = {
        0, 1, 3, // 1st triangle
        1, 2, 3  // 2nd triangle
    };

    // Textures
    /*float texCoords[] = {
        // x,    y
        0.0f, 0.0f, // Bottom left
        1.0f, 0.0f, // Bottom right
        1.0f, 1.0f  // Top right
    };
     */

    // Set up textures
    bool success;

    Texture texture1 = Texture();
    texture1.setParams();
    success = texture1.load(
        FileSystem::getPath("resources/textures/container.jpg").c_str(),
        GL_RGB,
        false
    );
    if (!success) {
        std::cout << "Failed to load texture:\n" << strerror(errno)
            << std::endl;
    }
    
    Texture texture2 = Texture();
    texture2.setParams();
    success = texture2.load(
        FileSystem::getPath("resources/textures/awesomeface.png").c_str(),
        GL_RGBA,
        true
    );
    if (!success) {
        std::cout << "Failed to load texture:\n" << strerror(errno)
            << std::endl;
    }

    // Set up shaders
    std::stringstream vertexPathStream, fragmentPathStream;
    
    vertexPathStream << logl_root
        << "/src/4.learnOpenGL/4.transformations/vertex.vs";
    fragmentPathStream << logl_root
        << "/src/4.learnOpenGL/4.transformations/fragment.fs";

    Shader shader(
        vertexPathStream.str().c_str(),
        fragmentPathStream.str().c_str()
    );

    // Create vertex buffer object
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    
    // Create vertex array object
    unsigned int vao;
    glGenVertexArrays(1, &vao);

    // Create element buffer object
    unsigned int ebo;
    glGenBuffers(1, &ebo);

    // Initialise

    // Bind vao
    glBindVertexArray(vao);
    // Bind to array buffer target
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Copy the vertex data into the buffer
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(verticies),
        verticies,
        GL_STATIC_DRAW // The data is set once and used many times
    );
    // Copy the indicies array in a element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indicies),
        indicies,
        GL_STATIC_DRAW
    );
    // Set vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(
        0, // Location of the vertex attribute
        3, // Size of the vertex attribute (its a vec3)
        GL_FLOAT, // Type of data
        GL_FALSE, // Normalising integers, not relevant to us
        8 * sizeof(float), // Stride - space between vertex attributes
        (void*)0 // Offset of where position data starts
    );
    glEnableVertexAttribArray(0);
    // Colour attribute
    glVertexAttribPointer(
        1, // Colour is the 2nd part of the vertex, index 1
        3, // Size of attribute
        GL_FLOAT, // Type
        GL_FALSE, // Irrelevant
        8 * sizeof(float), // Same stride as pos attribute
        (void*)(3 * sizeof(float)) // Offset - Start after the position
    );
    glEnableVertexAttribArray(1);
    // Texture coords attribute
    glVertexAttribPointer(
        2, // Textur coords is 3rd part of the vertex
        2, // Size of 2 (x, y)
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float), // Stride
        (void*)(6 * sizeof(float)) // Offset
    );
    glEnableVertexAttribArray(2);

    // Set texture samplers for each texture
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {

        // Handle input
        processInput(window);

        // Bg fill colour and clear
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate and bind textures
        glActiveTexture(GL_TEXTURE0);
        texture1.bind();
        glActiveTexture(GL_TEXTURE1);
        texture2.bind();

        // Use the shader program
        shader.use();

        // Bind vao
        glBindVertexArray(vao);

        // Bind ebo
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        // Draw the triangle!
        glDrawElements(
            GL_TRIANGLES, // Primitive we are drawing
            6, // Number of elements to draw (6 verticies)
            GL_UNSIGNED_INT, // Type of the incicies
            0 // Offset in the ebo
        );

        // Finish up

        // Unbind the vao
        glBindVertexArray(0);

        // Swap the colour buffer
        glfwSwapBuffers(window);

        // Check for event triggers eg. kb/m input
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    
    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    // Handle Esc
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Handle wireframe when `w` is pressed
    static bool wireframe = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}
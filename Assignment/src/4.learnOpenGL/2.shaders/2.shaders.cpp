// Local Headers
#include "2.shaders.h"

// Get the root directory that defines logl_root
#include "root_directory.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

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
        //  positions            colours
        //  x,     y,     z,     r,    g,    b
         0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 0.0f,  // top right
         0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f,  // bottom left
        -0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 1.0f   // top left
    };
    unsigned int indicies[] = {
        0, 1, 3, // 1st triangle
        1, 2, 3  // 2nd triangle
    };

    std::stringstream vertexPathStream, fragmentPathStream;
    
    vertexPathStream << logl_root
        << "/src/4.learnOpenGL/2.shaders/2.shader.vs";
    fragmentPathStream << logl_root
        << "/src/4.learnOpenGL/2.shaders/2.shader.fs";

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
        6 * sizeof(float), // Stride - space between vertex attributes
        (void*)0 // Offset of where position data starts
    );
    glEnableVertexAttribArray(0);
    // Colour attribute
    glVertexAttribPointer(
        1, // Colour is the 2nd part of the vertex, index 1
        3, // Size of attribute
        GL_FLOAT, // Type
        GL_FALSE, // Irrelevant
        6 * sizeof(float), // Same stride as pos attribute
        (void*)(3 * sizeof(float)) // Offset - Start after the position
    );
    glEnableVertexAttribArray(1);

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {

        // Handle input
        processInput(window);

        // Bg fill colour
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        shader.use();

        // Change colour by setting uniform value
        /*
        glUniform4f(
            glGetUniformLocation(shaderProgram, "ourColour"),
            0.0f,
            (sin(glfwGetTime()) / 2.0f) + 0.5f, // Fluctuates between 
            0.0f,
            1.0f
        );
         */

        // Bind vao
        glBindVertexArray(vao);

        // Bind ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        // Draw the triangle!
        glDrawElements(
            GL_TRIANGLES, // Primitive we are drawing
            6, // Number of elements to draw (6 verticies)
            GL_UNSIGNED_INT, // Type of the incicies
            0 // Offset in the ebo
        );
        /* Old draw arrays method
        glDrawArrays(
            GL_TRIANGLES, // Primitive we are drawing
            0, // Starting idx
            3 // Number of verticies to draw
        );
         */

        // Finish up

        // Unbind the vao
        glBindVertexArray(0);

        // Swap the colour buffer
        glfwSwapBuffers(window);

        // Check for event triggers eg. kb/m input
        glfwPollEvents();
    }
    
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
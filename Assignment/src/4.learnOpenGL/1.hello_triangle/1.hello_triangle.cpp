// Local Headers
#include "1.hello_triangle.h"

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

const char *vertexShaderSrc =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0"
;

const char *fragmentShaderSrc =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0"
;

int main(int argc, char* argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); macOS only
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
        //  x,     y,     z
         0.5f,  0.5f,  0.0f, // top right
         0.5f, -0.5f,  0.0f, // bottom right
        -0.5f, -0.5f,  0.0f, // bottom left
        -0.5f,  0.5f,  0.0f  // top left
    };
    unsigned int indicies[] = {
        0, 1, 3, // 1st triangle
        1, 2, 3  // 2nd triangle
    };

    // Error checking values
    int success;
    char infoLog[512];

    // Create vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attatch src code
    glShaderSource(
        vertexShader,
        1, // No. of strings in vertexShaderSrc
        &vertexShaderSrc, // Vertex shader source code
        nullptr
    );
    // Compile
    glCompileShader(vertexShader);
    // Compilation error checking
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << std::endl;
    }
    
    // Create fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Attatch src code
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    // Compile
    glCompileShader(fragmentShader);
    // Compilation error checking
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << std::endl;
    }

    // Create shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    // Attatch and link compiled shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Error checking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n"
            << infoLog << std::endl;
    }

    // Delete the compiled shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);   

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
    glVertexAttribPointer(
        0, // Location of the vertex attribute
        3, // Size of the vertex attribute (its a vec3)
        GL_FLOAT, // Type of data
        GL_FALSE, // Normalising integers, not relevant to us
        3 * sizeof(float), // Stride - space between vertex attributes
        (void*)0 // Offset of where position data starts
    );
    glEnableVertexAttribArray(0);

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {

        // Handle input
        processInput(window);

        // Bg fill colour
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

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

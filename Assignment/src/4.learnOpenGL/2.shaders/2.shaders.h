// Preprocessor Directives
#ifndef HELLO_TRIANGLE_H
#define HELLO_TRIANGLE_H
#pragma once

// System Headers
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
//#include <btBulletDynamicsCommon.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Util headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
//     #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Define Some Constants
const int mWidth = 1280;
const int mHeight = 800;

// Shader class
class Shader {

public:
    // Shader program ID
    unsigned int id;

    // Constructor
    Shader(const char* vertexPath, const char* fragmentPath) {
        char* vertexShaderCode = nullptr;
        char* fragmentShaderCode = nullptr;
        readShaderFiles(
            vertexPath, fragmentPath, // Sources
            vertexShaderCode, fragmentShaderCode // Returns
        );
        compileShaders(vertexShaderCode, fragmentShaderCode);
    }

    // Use/activate the shader
    void use() {
        glUseProgram(this->id);
    }

    // Utility uniform functions
    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
    }

    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
    }

private:

    void readShaderFiles(
        const char* vertexPath,
        const char* fragmentPath,
        const char* outVertexCode,
        const char* outFragmentCode
    ) {
        std::string vertexSrc;
        std::string fragmentSrc;
        std::ifstream vertexFile;
        std::ifstream fragmentFile;
        std::stringstream vertexStream;
        std::stringstream fragmentStream;

        // Set up ifstreams to throw exceptions
        vertexFile.exceptions(
            std::ifstream::failbit | std::ifstream::badbit
        );
        fragmentFile.exceptions(
            std::ifstream::failbit | std::ifstream::badbit
        );

        try {
            // Open files
            vertexFile.open(vertexPath);
            fragmentFile.open(fragmentPath);

            // Read file's buffer contents into streams
            vertexStream << vertexFile.rdbuf();
            fragmentStream << fragmentFile.rdbuf();

            // Close files
            vertexFile.close();
            fragmentFile.close();

            // Convert streams to strings
            vertexSrc = vertexStream.str();
            fragmentSrc = fragmentStream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
        }
        
        outVertexCode = vertexSrc.c_str();
        outFragmentCode = fragmentSrc.c_str();

        // TODO debug
        std::cout << "DEBUG: vertex and fragment src files\n"
            << outVertexCode << std::endl
            << outFragmentCode << std::endl;
    }

    void compileShaders(
        const char* vertexShaderSrc,
        const char* fragmentShaderSrc
    ) {
        // TODO debug
        std::cout << "DEBUG: vertex and fragment src files\n"
            << vertexShaderSrc << std::endl
            << fragmentShaderSrc << std::endl;

        #define LOG_SIZE 512
        unsigned int vertexShader, fragmentShader;
        int success;
        char infoLog[LOG_SIZE];

        // Compile vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);

        // Check for errors
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, LOG_SIZE, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
        }

        // Compile fragment shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
        glCompileShader(fragmentShader);

        // Check for errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, LOG_SIZE, nullptr, infoLog);
            std::cout << "ERROR::FRAGMENT::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
        }

        // Shader program
        this->id = glCreateProgram();
        glAttachShader(this->id, vertexShader);
        glAttachShader(this->id, fragmentShader);
        glLinkProgram(this->id);

        // Check for errors
        glGetProgramiv(this->id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->id, LOG_SIZE, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
        }

        // Delete shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
};

#endif 

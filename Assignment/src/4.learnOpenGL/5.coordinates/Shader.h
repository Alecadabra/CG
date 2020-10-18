#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <stb_image.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Error message log char[] size
#define LOG_SIZE 512

// Shader class
class Shader {

public:
    // Shader program ID
    unsigned int id;

    // Constructor
    Shader(const char* vertexPath, const char* fragmentPath) {
        std::string vertexShaderCode;
        std::string fragmentShaderCode;
        readShaderFiles(
            vertexPath, fragmentPath, // Sources
            vertexShaderCode, fragmentShaderCode // Returns
        );
        compileShaders(vertexShaderCode.c_str(), fragmentShaderCode.c_str());
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

    void setMat4(
        const std::string& name,
        const glm::mat4& value
    ) const {
        glUniformMatrix4fv(
            glGetUniformLocation(this->id, name.c_str()),
            1, // Number of matricies
            GL_FALSE, // Transpose the matrix
            glm::value_ptr(value) // Convert to glm's matrix storage
        );
    }

private:

    void readShaderFiles(
        const char* vertexPath, const char* fragmentPath, //  Value
        std::string& outVertexCode, std::string& outFragmentCode // Reference
    ) {
        std::ifstream vertexFile, fragmentFile; // File streams
        std::stringstream vertexStream, fragmentStream; // String streams

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
            outVertexCode = vertexStream.str();
            outFragmentCode = fragmentStream.str();
            
        } catch (std::ifstream::failure&) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n"
                << "Error: " << strerror(errno) << std::endl;
        }
    }

    void compileShaders(
        const char* vertexShaderSrc,
        const char* fragmentShaderSrc
    ) {
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
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
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
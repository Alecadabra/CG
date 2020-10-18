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
#include <GLFW/glfw3.h>

// Util headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
//     #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Forward declarations
void framebuffer_size_callback(GLFWwindow *, int, int);
void processInput(GLFWwindow *);

#endif 

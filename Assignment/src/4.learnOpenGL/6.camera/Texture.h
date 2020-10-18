#pragma once

#include <glad/glad.h>

#include <stb_image.h>

class Texture {

private:

    unsigned int textureObject;

    int textureWrap = GL_MIRRORED_REPEAT;
    int textureFilterMin = GL_NEAREST;
    int textureFilterMax = GL_LINEAR;

public:

    Texture() {
        glGenTextures(1, &textureObject);
        bind();
    }

    void setParams() {
        // Set texture wrapping
        glTexParameteri(
            GL_TEXTURE_2D,     // Texture target (2D)
            GL_TEXTURE_WRAP_S, // Option we want to set and which texture axis
            textureWrap // Repeat textures and mirror
        );
        // Same for T axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);
        // Set texture filtering (ie. nearest neighbor or bilinear)
        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER, // When Scaling down
            textureFilterMin // Use nearest neighbor
        );
        // Same for scaling up (magnifying)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilterMax);
    }

    bool load(const char* path, int format, bool flip) {

        int width, height, nChannels;
        unsigned char* data;
        bool success;

        stbi_set_flip_vertically_on_load(flip);

        data = stbi_load(path,&width, &height, &nChannels, 0);
        // Error checking
        if (data) {
            // Generate texture
            glTexImage2D(
                GL_TEXTURE_2D, // Generate a 2d texture
                0, // Mipmap level
                GL_RGB, // Texture storage format
                width, // Width of texture
                height, // Height of texture
                0, // Legacy
                format, // Src image format
                GL_UNSIGNED_BYTE, // Src image format
                data // Image data
            );
            glGenerateMipmap(GL_TEXTURE_2D);
            success = true;

        } else {
            success = false;
        }
        // Free image data
        stbi_image_free(data);
        return success;
    }

    void bind() { glBindTexture(GL_TEXTURE_2D, this->textureObject); }

    unsigned int getTextureObject() { return this->textureObject; }
    int getTextureWrap() { return this->textureWrap; }
    int getTextureFilterMin() { return this->textureFilterMin; }
    int getTextureFilterMax() { return this->textureFilterMax; }

    void setTextureWrap(int textureWrap) { 
        this->textureWrap = textureWrap; 
    }
    void setTextureFilterMin(int textureFilterMin) {
        this->textureFilterMin = textureFilterMin;
    }
    void setTextureFilterMax(int textureFilterMax) {
        this->textureFilterMax = textureFilterMax;
    }
};
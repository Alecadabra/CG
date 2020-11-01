#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include "camera.h"

#include <iostream>
#include <string>

// Box coordinate with 36 vertices.
// Every 3 coordinates will form 1 triangle.
// The last 2 columns represent texture coordinate for mapping.
float box[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

// Represents a box primitive
class Box {

public:

	// Texures
	unsigned int* diffuseTex;
	unsigned int* specularTex;

	// Transformations
	glm::vec3 scale;
	glm::vec3 translate;
	float xAngle, yAngle, zAngle;

	// Optimization for boxes that do not change transformation
	bool staticTransformation;
	bool staticModelCalculated = false;
	glm::mat4 staticModel; // Lazily calculated on render

	Box(
		unsigned int* diffuseTex,
		unsigned int* specularTex,
		bool staticTransformation = false
	) {
		this->diffuseTex = diffuseTex;
		this->specularTex = specularTex;

		this->xAngle = 0.0f;
		this->yAngle = 0.0f;
		this->zAngle = 0.0f;
		this->scale = glm::vec3();
		this->translate = glm::vec3();

		this->staticTransformation = staticTransformation;
	}

	// Activate textures
	void activateTextures() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *diffuseTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *specularTex);
	}

	// Apply transformations to a single matrix
	glm::mat4 transform() {

		glm::mat4 model = glm::translate(glm::mat4(), translate);

		if (this->yAngle != 0.0f) {
			model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (this->xAngle != 0.0f) {
			model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (this->zAngle != 0.0f) {
			model = glm::rotate(model, glm::radians(zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		model = glm::scale(model, scale);
		
		return model;
	}

	// Bind to vertex array object
	void bind(unsigned int vao) {
		glBindVertexArray(vao);
	}

	// Draw box primitive
	void draw() {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Render the box - bind, activate textures, apply model, draw.
	void render(Shader shader, unsigned int vao, glm::mat4* model = nullptr) {
		if (staticTransformation && !staticModelCalculated) {
			// Calulcate model (once only)
			this->staticModel = transform();
			this->staticModelCalculated = true;
		}

		glm::mat4 localModel;
		if (model != nullptr) {
			// Model is specified
			localModel = *model;
		} else if (staticTransformation) {
			// Model is static
			localModel = staticModel;
		} else {
			// Model is dynamically calculated
			localModel = transform();
		}

		bind(vao);
		activateTextures();
		shader.setMat4("model", localModel);
		draw();
	}

	// Gets the negative side bounds of the box
	glm::vec3 getNegativeBounds() {
		float negX = (-scale.x) / 2 + translate.x;
		float negY = (-scale.y) / 2 + translate.y;
		float negZ = (-scale.z) / 2 + translate.z;
		return glm::vec3(negX, negY, negZ);
	}
	
	// Gets the positive side bounds of the box
	glm::vec3 getPositiveBounds() {
		float posX = scale.x / 2 + translate.x;
		float posY = scale.y / 2 + translate.y;
		float posZ = scale.z / 2 + translate.z;
		return glm::vec3(posX, posY, posZ);
	}
};

#include "assignment.h"

// textures
unsigned int tex_wood_diffuse, tex_street_diffuse, tex_grass_diffuse, tex_marble_diffuse, tex_curtin_diffuse;
unsigned int tex_wood_specular, tex_street_specular, tex_grass_specular, tex_marble_specular, tex_curtin_specular;

unsigned int tex_red_dark_diffuse, tex_red_bright_diffuse, tex_red_diffuse, tex_green_diffuse, tex_blue_diffuse;
unsigned int tex_red_dark_specular, tex_red_bright_specular, tex_red_specular, tex_green_specular, tex_blue_specular;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float NEAR_CLIP = 0.1f;
const float FAR_CLIP = 300.0f;

const float cam_height = 0.8f;

const float hitbox_pad = 0.25f;

// camera
Camera camera(glm::vec3(0.0f, cam_height, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// lighting
glm::vec3 light_pos(0.0f, 1.0f, 0.1f);

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

int INPUT_DELAY = 0;
int INPUT_MAX = 20;

float light_radius = 1.0f;

//Toggle (Animation or states)
bool BUTTON_PRESSED = false;
bool BUTTON_CLOSE_ENOUGH = false;

bool SHOW_COORDINATE = false;

bool PERSPECTIVE_PROJECTION = true;

bool EXTRA_BRIGHT = false;

//Animation Variables
float curtin_rotate_y = 0.0;
float curtin_translate_y = 0.0;

// Boxes
Box floorBox(&tex_marble_diffuse, &tex_marble_specular);
Box rightWall(&tex_marble_diffuse, &tex_marble_specular);
Box leftWall(&tex_marble_diffuse, &tex_marble_specular);
Box buttonBox(nullptr, nullptr);
Box curtin(&tex_curtin_diffuse, &tex_curtin_specular);

// Which door is next
DoorStage doorStage = WOOD;

// Door boxes
Box woodDoor(&tex_wood_diffuse, &tex_wood_specular);
Box greenDoor(&tex_green_diffuse, &tex_green_specular);

Box* doorBoxes[] = {
	&woodDoor, &greenDoor
};
int doorBoxesLen = 2;

// Door animations
float doorAnim[] = {
	0.0f, 0.0f
};

int main() {

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(
		SCR_WIDTH, SCR_HEIGHT, "Alec OpenGL Assignment", nullptr, nullptr
	);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return EXIT_FAILURE;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lighting_shader(
		FileSystem::getPath("src/assignment/assignment/vertex.vs").c_str(),
		FileSystem::getPath("src/assignment/assignment/fragment.fs").c_str()
	);
	Shader lamp_shader(
		FileSystem::getPath("src/assignment/assignment/lampVertex.vs").c_str(),
		FileSystem::getPath("src/assignment/assignment/lampFragment.fs").c_str()
	);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// Vertex buffer/array objects
	unsigned int VBO_box, VAO_box;
	glGenVertexArrays(1, &VAO_box);
	glGenBuffers(1, &VBO_box);

	glBindVertexArray(VAO_box);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_box);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);

	//vertex coordinates
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0
	);
	glEnableVertexAttribArray(0);
	//normal vectors
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))
	);
	glEnableVertexAttribArray(1);
	//texture coordinates
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))
	);
	glEnableVertexAttribArray(2);


	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int VAO_light;
	glGenVertexArrays(1, &VAO_light);
	glBindVertexArray(VAO_light);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_box);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0
	);
	glEnableVertexAttribArray(0);



	// load and create textures
	// -------------------------
	tex_wood_diffuse = loadTexture(FileSystem::getPath("resources/textures/wood2.jpg").c_str());
	tex_wood_specular = loadTexture(FileSystem::getPath("resources/textures/wood2_specular.jpg").c_str());
	tex_street_diffuse = loadTexture(FileSystem::getPath("resources/textures/street.png").c_str());
	tex_street_specular = loadTexture(FileSystem::getPath("resources/textures/street_specular.png").c_str());
	tex_grass_diffuse = loadTexture(FileSystem::getPath("resources/textures/grass.jpg").c_str());
	tex_grass_specular = loadTexture(FileSystem::getPath("resources/textures/grass_specular.jpg").c_str());
	tex_marble_diffuse = loadTexture(FileSystem::getPath("resources/textures/marble.jpg").c_str());
	tex_marble_specular = loadTexture(FileSystem::getPath("resources/textures/marble_specular.jpg").c_str());
	tex_curtin_diffuse = loadTexture(FileSystem::getPath("resources/textures/curtin.jpg").c_str());
	tex_curtin_specular = loadTexture(FileSystem::getPath("resources/textures/curtin_specular.jpg").c_str());

	tex_red_dark_diffuse = loadTexture(FileSystem::getPath("resources/textures/red_dark.jpg").c_str());
	tex_red_dark_specular = loadTexture(FileSystem::getPath("resources/textures/red_dark_specular.jpg").c_str());
	tex_red_bright_diffuse = loadTexture(FileSystem::getPath("resources/textures/red_bright.jpg").c_str());
	tex_red_bright_specular = loadTexture(FileSystem::getPath("resources/textures/red_bright_specular.jpg").c_str());
	tex_red_diffuse = loadTexture(FileSystem::getPath("resources/textures/red.jpg").c_str());
	tex_red_specular = loadTexture(FileSystem::getPath("resources/textures/red_specular.jpg").c_str());
	tex_green_diffuse = loadTexture(FileSystem::getPath("resources/textures/green.jpg").c_str());
	tex_green_specular = loadTexture(FileSystem::getPath("resources/textures/green_specular.jpg").c_str());
	tex_blue_diffuse = loadTexture(FileSystem::getPath("resources/textures/blue.jpg").c_str());
	tex_blue_specular = loadTexture(FileSystem::getPath("resources/textures/blue_specular.jpg").c_str());

	
	//shader configuration -----------------------------------------------------
	lighting_shader.use();
	lighting_shader.setInt("material.diffuse", 0);
	lighting_shader.setInt("material.specular", 1);


	// set static transformations ----------------------------------------------

	// right wall
	rightWall.translate = glm::vec3(1.5f, 1.5f, -3.0f);
	rightWall.scale = glm::vec3(0.001f, 3.0f, 18.0f);

	// left wall
	leftWall.translate = glm::vec3(-1.5f, 1.5f, -3.0f);
	leftWall.scale = glm::vec3(0.001f, 3.0f, 18.0f);

	// floor
	floorBox.translate = glm::vec3(0.0f, 0.0f, -3.0f);
	floorBox.scale = glm::vec3(3.0f, 0.001f, 18.0f);

	woodDoor.scale = glm::vec3(3.0f, 3.0f, 1.0f);

	greenDoor.scale = glm::vec3(3.0f, 3.0f, 1.0f);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		delta_time = currentFrame - last_frame;
		last_frame = currentFrame;

		//update delay countdown
		INPUT_DELAY = max(0, INPUT_DELAY - 1);

		// input
		// -----
		process_input(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// activate shader
		lighting_shader.use();
		lighting_shader.setVec3("light.position", light_pos);
        lighting_shader.setVec3("viewPos", camera.Position);

		// light properties
		if (EXTRA_BRIGHT) {
			lighting_shader.setVec3("light.ambient", 0.6f, 0.6f, 0.6f);
		} else {
			lighting_shader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
		}

		float brightness = abs(cos(glfwGetTime()) / 2 ) + 0.8f;
		lighting_shader.setVec3("light.diffuse", glm::vec3(brightness * 0.8f));
		lighting_shader.setVec3("light.specular", glm::vec3(brightness));

		/*if(BUTTON_PRESSED == true)
		{
			lighting_shader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
			lighting_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		}
		else
		{
			lighting_shader.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
			lighting_shader.setVec3("light.specular", 0.0f, 0.0f, 0.0f);
		}*/

		// material properties
        lighting_shader.setFloat("material.shininess", 65.0f);
		lighting_shader.setFloat("radius", light_radius);

		// pass projection matrix to shader
		glm::mat4 projection;
		if (PERSPECTIVE_PROJECTION) {
			projection = glm::perspective(
				glm::radians(camera.Zoom),
				(float)SCR_WIDTH / (float)SCR_HEIGHT,
				NEAR_CLIP, FAR_CLIP
			);
		} else {
			projection = glm::ortho(
				-1.0f, 1.0f, -1.0f, 1.0f,
				NEAR_CLIP, FAR_CLIP
			);
		}
		lighting_shader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		lighting_shader.setMat4("view", view);


		//Draw objects
		//------------------------------------------------------------------------------------------


		//Coordinate System
		if(SHOW_COORDINATE == true)
		{
			glm::vec3 coord_scales[] = {
				glm::vec3( 100.0f,  0.02f,  0.02f),	//X
				glm::vec3( 0.02f,  100.0f,  0.02f),	//Y
				glm::vec3( 0.02f,  0.02f,  100.0f),	//Z
			};

			glBindVertexArray(VAO_box);
			
			for(int tab = 0; tab < 3; tab++)
			{	
				if(tab == 0) { // x
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex_red_diffuse);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, tex_red_specular);
				}
				if(tab == 1) { // y
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex_green_diffuse);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, tex_green_specular);
				}
				if(tab == 2) { // z
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex_blue_diffuse);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, tex_blue_specular);
				}

				glm::mat4 model = glm::mat4();
				model = glm::scale(model, coord_scales[tab]);

				lighting_shader.setMat4("model", model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		// render static objects
		floorBox.render(lighting_shader, VAO_box);
		rightWall.render(lighting_shader, VAO_box);
		leftWall.render(lighting_shader, VAO_box);

		// Button
		unsigned int diffuseTex, specularTex;
		float buttonHeight;
		if (BUTTON_PRESSED) {
			diffuseTex = tex_red_bright_diffuse;
			specularTex = tex_red_bright_specular;
			buttonHeight = 0.51f;
		} else {
			diffuseTex = tex_red_dark_diffuse;
			specularTex = tex_red_dark_specular;
			buttonHeight = 0.53f;
		}
		buttonBox.diffuseTex = &diffuseTex;
		buttonBox.specularTex = &specularTex;
		buttonBox.scale = glm::vec3(0.12f,  0.12f,  0.12f);
		buttonBox.translate = glm::vec3(0.0f, buttonHeight, 0.0f);
		buttonBox.render(lighting_shader, VAO_box);

		BUTTON_CLOSE_ENOUGH = glm::length(camera.Position - glm::vec3(0.0f, 0.56f, 0.25f)) <= 1.6f;


		// Wood door
		{
			float yVal = woodDoor.scale.y / 2 + doorAnim[DoorStage::WOOD] * 2.0f;
			woodDoor.translate = glm::vec3(0, yVal, -5.0f);
		}
		doorAnimationStep(WOOD);
		woodDoor.render(lighting_shader, VAO_box);

		// Green door
		{
			float xVal = greenDoor.scale.x * -doorAnim[DoorStage::GREEN];
			float yVal = greenDoor.scale.y / 2 + sin(glm::radians(doorAnim[DoorStage::GREEN] * 180.0f));
			greenDoor.translate = glm::vec3(xVal, yVal, -10.0f);
		}
		greenDoor.angle = doorAnim[DoorStage::GREEN] * 90.0f;
		greenDoor.rotate = glm::vec3(0.0f, 0.0f, 1.0f);
		doorAnimationStep(GREEN);
		greenDoor.render(lighting_shader, VAO_box);

		//Curtin Logo
		curtin.translate = glm::vec3(0.0f, 0.9f + (0.1f * sin(curtin_translate_y * PI / 180.f)), -0.35f);
		curtin.angle = curtin_rotate_y;
		curtin.rotate = glm::vec3(0.0f, 1.0f, 0.0f);
		curtin.scale = glm::vec3(0.2f, 0.2f, 0.001f);
		curtin.render(lighting_shader, VAO_box);


		//transformation for animation
		if(BUTTON_PRESSED) {
			curtin_translate_y += 1.0f;
			curtin_rotate_y += 1.0f;
			if(abs(curtin_translate_y - 360.0f) <= 0.1f) curtin_translate_y = 0.0f;
			if(abs(curtin_rotate_y - 360.0f) <= 0.1f) curtin_rotate_y = 0.0f;
		}

		// Draw the light source
		lamp_shader.use();
		lamp_shader.setMat4("projection", projection);
		lamp_shader.setMat4("view", view);
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3(0.01f)); // a smaller cube
		lamp_shader.setMat4("model", model);

		lamp_shader.setFloat("intensity", 1.0); /*
		if(BUTTON_PRESSED == true) lamp_shader.setFloat("intensity", 1.0);
		else lamp_shader.setFloat("intensity", 0.3); */

		glBindVertexArray(VAO_light);
		glDrawArrays(GL_TRIANGLES, 0, 36);





		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO_box);
	glDeleteBuffers(1, &VBO_box);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return EXIT_SUCCESS;
}

void doorAnimationStep(DoorStage stage) {
	if (doorStage > stage && doorAnim[stage] < 1.0f) {
		doorAnim[stage] = min(1.0f, doorAnim[stage] + delta_time);
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow *window) {
	// Window closing
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	float minX, maxX, minZ, maxZ;
	minX = leftWall.getPositiveBounds().x + hitbox_pad;
	maxX = rightWall.getNegativeBounds().x - hitbox_pad;
	minZ = floorBox.getNegativeBounds().z + hitbox_pad;
	maxZ = floorBox.getPositiveBounds().z - hitbox_pad;

	for (int i = 0; i < doorBoxesLen; i++) {
		glm::vec3 wallMin = doorBoxes[i]->getNegativeBounds();
		glm::vec3 wallMax = doorBoxes[i]->getPositiveBounds();

		if (wallMin.x < camera.Position.x && camera.Position.x > wallMax.x) {
			// Make sure camera is within the x range of the wall

			if (camera.Position.z > wallMax.z && camera.Position.y > wallMin.y) {
				// Try to walk through from +ve z side
				minZ = max(minZ, wallMax.z + hitbox_pad);
			}
			if (camera.Position.z < wallMin.z && camera.Position.y > wallMin.y) {
				// Try to walk through from -ve z side
				maxZ = min(maxZ, wallMin.z - hitbox_pad);
			}
		}
	}

	if (woodDoor.scale.y / 2 + woodDoor.translate.y < cam_height) {
		// Player cannot fit under door
		minZ = woodDoor.scale.z / 2 + woodDoor.translate.z;
	}
	

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, delta_time, minX, maxX, minZ, maxZ);
	}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, delta_time, minX, maxX, minZ, maxZ);
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, delta_time, minX, maxX, minZ, maxZ);
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, delta_time, minX, maxX, minZ, maxZ);
	}

	//toggle button
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && INPUT_DELAY == 0 && BUTTON_CLOSE_ENOUGH == true) {
		INPUT_DELAY = INPUT_MAX;
		BUTTON_PRESSED = !BUTTON_PRESSED;
		doorStage = GREEN;
	}
	//toggle button
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && INPUT_DELAY == 0) {
		INPUT_DELAY = INPUT_MAX;
		doorStage = OTHER;
	}

	//toggle coordinate visibility
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && INPUT_DELAY == 0) {
		INPUT_DELAY = INPUT_MAX;
		SHOW_COORDINATE = !SHOW_COORDINATE;
	}

	// toggle projection mode
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && INPUT_DELAY == 0) {
		INPUT_DELAY = INPUT_MAX;
		PERSPECTIVE_PROJECTION = !PERSPECTIVE_PROJECTION;
	}

	// toggle extra brightness
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && INPUT_DELAY == 0) {
		INPUT_DELAY = INPUT_MAX;
		EXTRA_BRIGHT = !EXTRA_BRIGHT;
	}

	// increase light brightness radius
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		light_radius = min(2.0f, light_radius + delta_time * 2.0f);
	}

	// decrease light brightness radius
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		light_radius = max(0.0f, light_radius - delta_time * 2.0f);
	}
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const * path) {

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {

		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		} else if (nrComponents == 3) {
			format = GL_RGB;
		} else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
#include "assignment.h"

// textures
unsigned int tex_wood_diffuse, tex_brickwall_diffuse, tex_metal_diffuse, tex_marble2_diffuse, tex_street_diffuse, tex_grass_diffuse, tex_marble_diffuse, tex_curtin_diffuse;
unsigned int tex_wood_specular, tex_brickwall_specular, tex_metal_specular, tex_marble2_specular, tex_street_specular, tex_grass_specular, tex_marble_specular, tex_curtin_specular;

unsigned int tex_red_dark_diffuse, tex_red_bright_diffuse, tex_red_diffuse, tex_green_diffuse, tex_blue_diffuse;
unsigned int tex_red_dark_specular, tex_red_bright_specular, tex_red_specular, tex_green_specular, tex_blue_specular;

unsigned int tex_marble2_graffiti_diffuse, tex_graffiti_2_diffuse, tex_graffiti_3_diffuse, tex_graffiti_4_diffuse;
unsigned int tex_marble2_graffiti_specular, tex_graffiti_2_specular, tex_graffiti_3_specular, tex_graffiti_4_specular;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float NEAR_CLIP = 0.1f;
const float FAR_CLIP = 300.0f;

const float cam_height = 0.8f;

const float hitbox_pad = 0.25f;
const float button_range = 0.8f;

// camera
Camera camera(glm::vec3(0.0f, cam_height, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

int INPUT_DELAY = 0;
int INPUT_MAX = 20;

float light_radius = 1.2f;
bool lamp_carrying = false;
glm::vec3 light_initial_pos = glm::vec3(0.0f, 0.4f, -0.5f);
glm::vec3 light_pos = light_initial_pos;

//Toggle (Animation or states)
bool buttonPressed[] = {
	false,
	false,
	false
};
int buttonPressedLen = 3;

bool SHOW_COORDINATE = false;

bool PERSPECTIVE_PROJECTION = true;

bool EXTRA_BRIGHT = false;

//Animation Variables
float curtin_rotate_y = 0.0;
float curtin_translate_y = 0.0;

// Boxes
Box floorBox(&tex_marble_diffuse, &tex_marble_specular);
Box roofBox(&tex_marble_diffuse, &tex_marble_specular);
Box rightWall(&tex_marble_diffuse, &tex_marble_specular);
Box leftWall(&tex_marble_diffuse, &tex_marble_specular);

// Button boxes
Box button1Box(nullptr, nullptr);
Box button2_brick(&tex_brickwall_diffuse, &tex_brickwall_specular);
Box button2_metal(&tex_metal_diffuse, &tex_metal_specular);
Box button2_wood(&tex_wood_diffuse, &tex_wood_specular);
Box button3_2(&tex_graffiti_2_diffuse, &tex_graffiti_2_specular);
Box button3_3(&tex_graffiti_3_diffuse, &tex_graffiti_3_specular);
Box button3_4(&tex_graffiti_4_diffuse, &tex_graffiti_4_specular);


// Door boxes
Box woodDoor(&tex_wood_diffuse, &tex_wood_specular);
Box brickDoor(&tex_brickwall_diffuse, &tex_brickwall_specular);
Box marbleDoor(&tex_marble2_graffiti_diffuse, &tex_marble2_graffiti_specular);
Box metalDoorBot(&tex_metal_diffuse, &tex_metal_specular);
Box metalDoorTop(&tex_metal_diffuse, &tex_metal_specular);
Box metalDoorLeft(&tex_metal_diffuse, &tex_metal_specular);
Box metalDoorRight(&tex_metal_diffuse, &tex_metal_specular);

// Which door is next
enum DoorStage {
	WOOD = 0,
	BRICK = 1,
	MARBLE = 2,
	METAL = 3,
	FINISH = 4
};
DoorStage doorStage = WOOD;

Box* doorBoxes[] = {
	&woodDoor, &brickDoor, &marbleDoor, &metalDoorBot
};
int doorBoxesLen = 4;

// Door animations
float doorAnim[] = {
	0.0f, 0.0f, 0.0f, 0.0f
};

void doorAnimationStep(DoorStage stage);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void computeBounds(float&, float&, float&, float&, float);
bool checkButtonRange(Box);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
unsigned int loadTexture(char const * path);

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
	tex_brickwall_diffuse = loadTexture(FileSystem::getPath("resources/textures/brickwall.jpg").c_str());
	tex_brickwall_specular = loadTexture(FileSystem::getPath("resources/textures/brickwall_specular.jpg").c_str());
	tex_metal_diffuse = loadTexture(FileSystem::getPath("resources/textures/metal.png").c_str());
	tex_metal_specular = loadTexture(FileSystem::getPath("resources/textures/metal_specular.png").c_str());
	tex_marble2_diffuse = loadTexture(FileSystem::getPath("resources/textures/marble2.jpg").c_str());
	tex_marble2_specular = loadTexture(FileSystem::getPath("resources/textures/marble2_specular.jpg").c_str());
	//tex_street_diffuse = loadTexture(FileSystem::getPath("resources/textures/street.png").c_str());
	//tex_street_specular = loadTexture(FileSystem::getPath("resources/textures/street_specular.png").c_str());
	//tex_grass_diffuse = loadTexture(FileSystem::getPath("resources/textures/grass.jpg").c_str());
	//tex_grass_specular = loadTexture(FileSystem::getPath("resources/textures/grass_specular.jpg").c_str());
	tex_marble_diffuse = loadTexture(FileSystem::getPath("resources/textures/marble.jpg").c_str());
	tex_marble_specular = loadTexture(FileSystem::getPath("resources/textures/marble_specular.jpg").c_str());
	tex_curtin_diffuse = loadTexture(FileSystem::getPath("resources/textures/curtin.jpg").c_str());
	tex_curtin_specular = loadTexture(FileSystem::getPath("resources/textures/curtin_specular.jpg").c_str());
	tex_red_dark_diffuse = loadTexture(FileSystem::getPath("resources/textures/red_dark.jpg").c_str());
	tex_red_dark_specular = loadTexture(FileSystem::getPath("resources/textures/red_dark_specular.jpg").c_str());
	tex_red_bright_diffuse = loadTexture(FileSystem::getPath("resources/textures/red_bright.jpg").c_str());
	tex_red_bright_specular = loadTexture(FileSystem::getPath("resources/textures/red_bright_specular.jpg").c_str());
	//tex_red_diffuse = loadTexture(FileSystem::getPath("resources/textures/red.jpg").c_str());
	//tex_red_specular = loadTexture(FileSystem::getPath("resources/textures/red_specular.jpg").c_str());
	//tex_green_diffuse = loadTexture(FileSystem::getPath("resources/textures/green.jpg").c_str());
	//tex_green_specular = loadTexture(FileSystem::getPath("resources/textures/green_specular.jpg").c_str());
	//tex_blue_diffuse = loadTexture(FileSystem::getPath("resources/textures/blue.jpg").c_str());
	//tex_blue_specular = loadTexture(FileSystem::getPath("resources/textures/blue_specular.jpg").c_str());
	tex_marble2_graffiti_diffuse = loadTexture(FileSystem::getPath("resources/textures/marble2_graffiti.jpg").c_str());
	tex_marble2_graffiti_specular = loadTexture(FileSystem::getPath("resources/textures/marble2_graffiti_specular.jpg").c_str());
	tex_graffiti_2_diffuse = loadTexture(FileSystem::getPath("resources/textures/graffiti_2_diffuse.jpg").c_str());
	tex_graffiti_2_specular = loadTexture(FileSystem::getPath("resources/textures/graffiti_2_specular.jpg").c_str());
	tex_graffiti_3_diffuse = loadTexture(FileSystem::getPath("resources/textures/graffiti_3_diffuse.jpg").c_str());
	tex_graffiti_3_specular = loadTexture(FileSystem::getPath("resources/textures/graffiti_3_specular.jpg").c_str());
	tex_graffiti_4_diffuse = loadTexture(FileSystem::getPath("resources/textures/graffiti_4_diffuse.jpg").c_str());
	tex_graffiti_4_specular = loadTexture(FileSystem::getPath("resources/textures/graffiti_4_specular.jpg").c_str());

	
	//shader configuration -----------------------------------------------------
	lighting_shader.use();
	lighting_shader.setInt("material.diffuse", 0);
	lighting_shader.setInt("material.specular", 1);


	// set static transformations ----------------------------------------------

	// right wall
	rightWall.translate = glm::vec3(1.5f, 1.5f, -8.0f);
	rightWall.scale = glm::vec3(0.001f, 3.0f, 30.0f);

	// left wall
	leftWall.translate = glm::vec3(-1.5f, 1.5f, -8.0f);
	leftWall.scale = glm::vec3(0.001f, 3.0f, 30.0f);

	// floor
	floorBox.translate = glm::vec3(0.0f, 0.0f, -8.0f);
	floorBox.scale = glm::vec3(3.0f, 0.001f, 30.0f);

	// roof
	roofBox.translate = glm::vec3(0.0f, 3.0f, -8.0f);
	roofBox.scale = glm::vec3(3.0f, 0.001f, 30.0f);

	// doors
	float doorZScale = 0.8f;
	woodDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);

	brickDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);
	brickDoor.rotate = glm::vec3(0.0f, 0.0f, 1.0f);

	marbleDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);

	{
		float holeSize = 1.2f;
		float width = (3.0f - holeSize) / 2;
		Box* metalDoors[] = {
			&metalDoorBot, &metalDoorTop, &metalDoorLeft, &metalDoorRight
		};
		for (int i = 0; i < 4; i++) {
			metalDoors[i]->scale = glm::vec3(3.0f, 3.0f, doorZScale);
			metalDoors[i]->translate = glm::vec3(0.0f, 1.5f, -20.0f);
		}
		// Make size correct
		metalDoorTop.scale.y = width;
		metalDoorBot.scale.y = width;
		metalDoorLeft.scale.x = width;
		metalDoorRight.scale.x = width;

		// Move to correct positions
		metalDoorTop.translate.y = 3.0f - width / 2;
		metalDoorBot.translate.y = width / 2;
		metalDoorLeft.translate.x = -1.5f + width / 2;
		metalDoorRight.translate.x = 1.5f - width / 2;

		// Make l/r skinny on z
		metalDoorLeft.scale.z /= 1.5f;
		metalDoorRight.scale.z /= 1.5f;

		// Make l/r short
		metalDoorLeft.scale.y = holeSize;
		metalDoorRight.scale.y = holeSize;
	}

	// buttons
	float buttonDimenScale = 0.12f;
	button1Box.scale = glm::vec3(buttonDimenScale);
	
	button2_brick.scale = glm::vec3(buttonDimenScale);
	button2_metal.scale = glm::vec3(buttonDimenScale);
	button2_wood.scale = glm::vec3(buttonDimenScale);

	{
		Box* buttons[] = { &button3_2, &button3_3, &button3_4 };
		for (int i = 0; i < 3; i++) {
			buttons[i]->scale = glm::vec3(buttonDimenScale);
			buttons[i]->angle = 90.0f;
			buttons[i]->rotate = glm::vec3(1.0f, 0.0f, 0.0f);
		}
	}


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
		if (lamp_carrying) {
			float minX, maxX, minZ, maxZ, pad = hitbox_pad / 4;
			computeBounds(minX, maxX, minZ, maxZ, pad);
			light_pos = camera.Position;
			light_pos.x += 0.5f * camera.Front.x + 0.1f * camera.Right.x;
			light_pos.x = med(minX, maxX, light_pos.x);
			light_pos.y += 0.3f * camera.Front.y - 0.15f;
			light_pos.z += 0.5f * camera.Front.z + 0.1f * camera.Right.z;
			light_pos.z = med(minZ, maxZ, light_pos.z);
		} else {
			double time = glfwGetTime();
			light_pos = glm::vec3(0.0f, 0.3f + sin(glm::radians(time * 180)) / 7.0f, -0.2f);
			lamp_carrying = glm::length(camera.Position - light_pos) < button_range;
		}
		lighting_shader.use();
		lighting_shader.setVec3("light.position", light_pos);
        lighting_shader.setVec3("viewPos", camera.Position);

		// light properties
		if (EXTRA_BRIGHT) {
			lighting_shader.setVec3("light.ambient", 0.6f, 0.6f, 0.6f);
		} else {
			lighting_shader.setVec3("light.ambient", 0.08f, 0.08f, 0.08f);
		}

		float brightness = abs(cos(glfwGetTime()) / 2 ) + 1.2f;
		lighting_shader.setVec3("light.diffuse", glm::vec3(brightness));
		lighting_shader.setVec3("light.specular", glm::vec3(brightness));

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
		roofBox.render(lighting_shader, VAO_box);
		rightWall.render(lighting_shader, VAO_box);
		leftWall.render(lighting_shader, VAO_box);

		// Button 1 (wood door)
		{
			unsigned int diffuseTex, specularTex;
			float buttonXOffset;
			if (buttonPressed[WOOD]) {
				diffuseTex = tex_red_dark_diffuse;
				specularTex = tex_red_dark_specular;
				buttonXOffset = 0.008f;
			} else {
				diffuseTex = tex_red_bright_diffuse;
				specularTex = tex_red_bright_specular;
				buttonXOffset = 0.02f;
			}
			button1Box.diffuseTex = &diffuseTex;
			button1Box.specularTex = &specularTex;
			float x = rightWall.getNegativeBounds().x - buttonXOffset;
			float y = camera.Position.y - 0.2f;
			float z = woodDoor.getPositiveBounds().z + 0.5f;
			button1Box.translate = glm::vec3(x, y, z);
		}
		button1Box.render(lighting_shader, VAO_box);

		// Button 2's (brick door)
		{
			Box* button2s[] = { &button2_brick, &button2_metal, &button2_wood };
			float buttonXOffset;
			if (buttonPressed[BRICK]) {
				buttonXOffset = 0.008f;
			} else {
				buttonXOffset = 0.02f;
			}
			float x = rightWall.getNegativeBounds().x - 0.02f;
			float y = camera.Position.y - 0.2f;
			float z = brickDoor.getPositiveBounds().z + 0.5f;
			for (int i = 1; i < 3; i++) {
				button2s[i]->translate = glm::vec3(x, y, z + i * 0.7f);
			}
			button2_brick.translate = glm::vec3(x - buttonXOffset, y, z);
			for (int i = 0; i < 3; i++) {
				button2s[i]->render(lighting_shader, VAO_box);
			}
		}

		// Button 3's (marble graffiti door)
		{
			Box initialDoor(&tex_brickwall_diffuse, &tex_brickwall_specular);
			initialDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);
			initialDoor.translate = glm::vec3(0.0f, initialDoor.scale.y / 2, -15.0f);
			Box* button3s[] = { &button3_2, &button3_3, &button3_4 };
			float buttonXOffset;
			if (buttonPressed[MARBLE]) {
				buttonXOffset = 0.008f;
			} else {
				buttonXOffset = 0.02f;
			}
			float x = rightWall.getNegativeBounds().x - 0.02f;
			float y = camera.Position.y - 0.2f;
			float z = initialDoor.getPositiveBounds().z + 0.5f;
			for (int i = 0; i < 3; i++) {
				button3s[i]->translate = glm::vec3(x, y, z + i * 0.7f);
			}
			button3_3.translate.x -= buttonXOffset;
			for (int i = 0; i < 3; i++) {
				button3s[i]->render(lighting_shader, VAO_box);
			}
		}


		// Wood door
		{
			float yVal = woodDoor.scale.y / 2 + sin(glm::radians(doorAnim[DoorStage::WOOD] * 90.0f)) * 2.0f;
			woodDoor.translate = glm::vec3(0, yVal, -5.0f);
		}
		doorAnimationStep(WOOD);
		woodDoor.render(lighting_shader, VAO_box);

		// Brick door
		{
			float anim = doorAnim[DoorStage::BRICK];
			float x = (brickDoor.scale.x - 0.3f) * anim * anim;
			float y = brickDoor.scale.y / 2 + sin(glm::radians(anim * anim * 180.0f));
			brickDoor.translate = glm::vec3(x, y, -10.0f);
			brickDoor.angle = anim * -90.0f;
			float deltaHeight = sin(glm::radians(anim * anim * 180)) * 2;
			brickDoor.scale.x = woodDoor.scale.x - deltaHeight;
			brickDoor.scale.y = woodDoor.scale.y - deltaHeight;
		}
		doorAnimationStep(BRICK);
		brickDoor.render(lighting_shader, VAO_box);

		// Marble graffiti door
		{
			float anim = doorAnim[DoorStage::MARBLE];
			glm::vec3 initialScale = glm::vec3(3.0f, 3.0f, doorZScale);
			glm::vec3 initialTranslate = glm::vec3(0.0f, initialScale.y / 2, -15.0f);
			marbleDoor.translate = initialTranslate;

			if (anim < 0.3f) {
				float localAnim = anim / 3.0f * 10.0f;
				marbleDoor.scale.x = initialScale.x - (initialScale.x - 1.0f) * sin(glm::radians(localAnim * 90.0f));
			} else if (anim < 0.6f) {
				float localAnim = (anim - 0.3f) / 3.0f * 10.0f;
				marbleDoor.scale.y = initialScale.y - (initialScale.y - 1.0f) * sin(glm::radians(localAnim * 90.0f));
			} else {
				float localAnim = (anim - 0.6f) / 3.0f * 10.0f;
				marbleDoor.angle = localAnim * 720.0f;
				marbleDoor.rotate = glm::vec3(0.0f, 0.0f, 1.0f);
				marbleDoor.translate.z = initialTranslate.z + initialTranslate.z * 4 * sin(glm::radians(localAnim * 45.0f));
			}
		}
		doorAnimationStep(MARBLE);
		marbleDoor.render(lighting_shader, VAO_box);


		// Metal door
		{
			doorAnimationStep(METAL);
			Box* metalDoors[] = {
			&metalDoorBot, &metalDoorTop, &metalDoorLeft, &metalDoorRight
			};
			for (int i = 0; i < 4; i++) {
				metalDoors[i]->render(lighting_shader, VAO_box);
			}
		}



		// Draw the light source
		glm::vec3 lampRotate = glm::vec3(0.0f, 1.0f, 0.0f);
		float lampAngle;
		if (lamp_carrying) {
			lampAngle = -camera.Yaw;
		} else {
			lampAngle = sin(glfwGetTime()) * 90.0f;
		}

		Box lampShaft(&tex_wood_diffuse, &tex_wood_specular);
		lampShaft.translate = glm::vec3(light_pos.x, light_pos.y - 0.045f, light_pos.z);
		lampShaft.scale = glm::vec3(0.013f, 0.08f, 0.013f);
		lampShaft.angle = lampAngle;
		lampShaft.rotate = lampRotate;
		lampShaft.render(lighting_shader, VAO_box);

		Box lampDetailTop(&tex_wood_diffuse, &tex_wood_specular);
		lampDetailTop.translate = glm::vec3(light_pos.x, light_pos.y - 0.01f, light_pos.z);
		lampDetailTop.scale = glm::vec3(0.02f, 0.01, 0.02f);
		lampDetailTop.angle = lampAngle;
		lampDetailTop.rotate = lampRotate;
		lampDetailTop.render(lighting_shader, VAO_box);

		Box lampDetailBot(&tex_wood_diffuse, &tex_wood_specular);
		lampDetailBot.translate = glm::vec3(light_pos.x, light_pos.y - 0.08f, light_pos.z);
		lampDetailBot.scale = glm::vec3(0.02f, 0.025, 0.02f);
		lampDetailBot.angle = lampAngle;
		lampDetailBot.rotate = lampRotate;
		lampDetailBot.render(lighting_shader, VAO_box);

		lamp_shader.use();
		lamp_shader.setFloat("intensity", 1.2);
		lamp_shader.setMat4("projection", projection);
		lamp_shader.setMat4("view", view);

		Box lampLight(&tex_red_bright_diffuse, &tex_red_specular);
		lampLight.translate = light_pos;
		lampLight.scale = glm::vec3(0.01f);
		lampLight.angle = lampAngle;
		lampLight.rotate = lampRotate;
		lampLight.render(lamp_shader, VAO_light);


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
		doorAnim[stage] = min(1.0f, doorAnim[stage] + delta_time / 2);
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow *window) {
	// Window closing
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Bounds
	float minX, maxX, minZ, maxZ;
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		computeBounds(minX, maxX, minZ, maxZ, hitbox_pad);
        camera.ProcessKeyboard(FORWARD, delta_time, minX, maxX, minZ, maxZ);
	}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		computeBounds(minX, maxX, minZ, maxZ, hitbox_pad);
        camera.ProcessKeyboard(BACKWARD, delta_time, minX, maxX, minZ, maxZ);
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		computeBounds(minX, maxX, minZ, maxZ, hitbox_pad);
        camera.ProcessKeyboard(LEFT, delta_time, minX, maxX, minZ, maxZ);
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		computeBounds(minX, maxX, minZ, maxZ, hitbox_pad);
        camera.ProcessKeyboard(RIGHT, delta_time, minX, maxX, minZ, maxZ);
	}

	// Buttons
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && INPUT_DELAY == 0) {
		if (checkButtonRange(button1Box)) {
			//toggle button 1 (wood door)
			INPUT_DELAY = INPUT_MAX;
			buttonPressed[WOOD] = true;
			if (doorStage == WOOD) {
				doorStage = BRICK;
			}
		} else if (checkButtonRange(button2_brick)) {
			//toggle button 2 (brick door)
			INPUT_DELAY = INPUT_MAX;
			buttonPressed[BRICK] = true;
			if (doorStage == BRICK) {
				doorStage = MARBLE;
			}
		} else if (checkButtonRange(button2_metal)) {
			// false button2 (metal)
			INPUT_DELAY = INPUT_MAX;
			button2_metal.diffuseTex = &tex_red_dark_diffuse;
			button2_metal.specularTex = &tex_red_dark_specular;
		} else if (checkButtonRange(button2_wood)) {
			// false button2 (wood)
			INPUT_DELAY = INPUT_MAX;
			button2_wood.diffuseTex = &tex_red_dark_diffuse;
			button2_wood.specularTex = &tex_red_dark_specular;
		} else if (checkButtonRange(button3_3)) {
			// toggle button 3 (marble graffiti door)
			INPUT_DELAY = INPUT_MAX;
			buttonPressed[MARBLE] = true;
			if (doorStage == MARBLE) {
				doorStage = METAL;
			}
		} else if (checkButtonRange(button3_2)) {
			// false button3 (1+2=2)
			INPUT_DELAY = INPUT_MAX;
			button3_2.diffuseTex = &tex_red_dark_diffuse;
			button3_2.specularTex = &tex_red_dark_specular;
		} else if (checkButtonRange(button3_4)) {
			// false button3 (1+2=4)
			INPUT_DELAY = INPUT_MAX;
			button3_4.diffuseTex = &tex_red_dark_diffuse;
			button3_4.specularTex = &tex_red_dark_specular;
		}
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
		light_radius = min(2.5f, light_radius + delta_time * 2.0f);
	}

	// decrease light brightness radius
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		light_radius = max(0.0f, light_radius - delta_time * 2.0f);
	}
}


void computeBounds(float& minX, float& maxX, float& minZ, float& maxZ, float pad) {
	// Start with bounds of static walls
	minX = leftWall.getPositiveBounds().x + pad;
	maxX = rightWall.getNegativeBounds().x - pad;
	minZ = floorBox.getNegativeBounds().z + pad;
	maxZ = floorBox.getPositiveBounds().z - pad;

	for (int i = 0; i < doorBoxesLen; i++) {
		glm::vec3 wallMin = doorBoxes[i]->getNegativeBounds();
		glm::vec3 wallMax = doorBoxes[i]->getPositiveBounds();

		if (wallMin.x < camera.Position.x && camera.Position.x < wallMax.x) {
			// Make sure camera is within the x range of the wall

			if (camera.Position.z > wallMax.z && camera.Position.y > wallMin.y) {
				// Try to walk through from +ve z side
				minZ = max(minZ, wallMax.z + pad);
			}
			if (camera.Position.z < wallMin.z && camera.Position.y > wallMin.y) {
				// Try to walk through from -ve z side
				maxZ = min(maxZ, wallMin.z - pad);
			}
		}

		if (wallMin.z < camera.Position.z && camera.Position.z < wallMax.z) {
			// Player has a wall to their left/right (z axis)

			if (camera.Position.x > wallMax.x && camera.Position.y > wallMin.y) {
				// Try to walk through from +ve z side
				minX = max(minX, wallMax.x + pad);
			}
			if (camera.Position.z < wallMin.x && camera.Position.y > wallMin.y) {
				// Try to walk through from -ve z side
				maxX = min(maxX, wallMin.x - pad);
			}
		}
	}
}

bool checkButtonRange(Box buttonBox) {
	return glm::length(camera.Position - buttonBox.translate) <= button_range;
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
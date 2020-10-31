#include "assignment.h"

// textures
unsigned int tex_wood_diffuse, tex_brickwall_diffuse, tex_metal_diffuse,
	tex_marble2_diffuse, tex_street_diffuse, tex_grass_diffuse,
	tex_marble_diffuse, tex_curtin_diffuse, tex_night_sky;
unsigned int tex_wood_specular, tex_brickwall_specular, tex_metal_specular,
	tex_marble2_specular, tex_street_specular, tex_grass_specular,
	tex_marble_specular, tex_curtin_specular;

unsigned int tex_red_dark_diffuse, tex_red_bright_diffuse, tex_red_diffuse,
	tex_green_diffuse, tex_blue_diffuse;
unsigned int tex_red_dark_specular, tex_red_bright_specular, tex_red_specular,
	tex_green_specular, tex_blue_specular;

unsigned int tex_marble2_graffiti_diffuse, tex_graffiti_2_diffuse,
	tex_graffiti_3_diffuse, tex_graffiti_4_diffuse;
unsigned int tex_marble2_graffiti_specular, tex_graffiti_2_specular,
	tex_graffiti_3_specular, tex_graffiti_4_specular;

unsigned int tex_win, tex_win_specular, tex_lose, tex_lose_specular;

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const float NEAR_CLIP = 0.1f;
const float FAR_CLIP = 50.0f;

const float HITBOX_PAD = 0.25f;
const float BTN_RANGE = 0.8f;

const int INPUT_MAX = 20;

const glm::vec3 LIGHT_INITIAL_POS = glm::vec3(0.0f, 0.4f, -0.5f);

// camera
float cam_height;
Camera camera(glm::vec3(0.0f));
float lastX;
float lastY;
bool firstMouse;
float fov;

Camera badGuy(glm::vec3(0.0f));

float sprintAnim;

// timing
float delta_time = 0.0f; // time between current frame and last frame
float last_frame = 0.0f;

int input_delay;

// Light
float light_radius;
bool lamp_carrying;
glm::vec3 light_pos;

//Toggle (Animation or states)
bool buttonPressed[4];
int buttonPressedLen = 4;

bool PERSPECTIVE_PROJECTION;

bool EXTRA_BRIGHT;

// Boxes
Box floorBox(&tex_marble_diffuse, &tex_marble_specular, true);
Box roofBox(&tex_marble_diffuse, &tex_marble_specular, true);
Box rightWall(&tex_marble_diffuse, &tex_marble_specular, true);
Box leftWall(&tex_marble_diffuse, &tex_marble_specular, true);
Box backWall(&tex_marble_diffuse, &tex_marble_specular, true);
Box frontWall(&tex_marble_diffuse, &tex_marble_specular, true);
Box winSquare(&tex_green_diffuse, &tex_metal_specular, true);

Box badGuyBox(&tex_red_bright_diffuse, &tex_marble_specular);
Box badGuyBoxBack(&tex_marble_diffuse, &tex_green_specular);
Box badGuyVert(&tex_marble_diffuse, &tex_green_specular);

// Button boxes
Box button1Box(&tex_green_diffuse, &tex_metal_specular);
Box button2_brick(&tex_brickwall_diffuse, &tex_brickwall_specular);
Box button2_metal(&tex_metal_diffuse, &tex_metal_specular);
Box button2_wood(&tex_wood_diffuse, &tex_wood_specular);
Box button3_2(&tex_graffiti_2_diffuse, &tex_graffiti_2_specular);
Box button3_3(&tex_graffiti_3_diffuse, &tex_graffiti_3_specular);
Box button3_4(&tex_graffiti_4_diffuse, &tex_graffiti_4_specular);
Box button4_1(&tex_grass_diffuse, &tex_grass_specular);
Box button4_2(&tex_curtin_diffuse, &tex_curtin_specular);
Box button4_3(&tex_street_diffuse, &tex_street_specular);


// Door boxes
Box woodDoor(&tex_wood_diffuse, &tex_wood_specular);
Box brickDoor(&tex_brickwall_diffuse, &tex_brickwall_specular);
Box marbleDoor(&tex_marble2_graffiti_diffuse, &tex_marble2_graffiti_specular);
Box metalDoorBot(&tex_metal_diffuse, &tex_metal_specular);
Box metalDoorTop(&tex_metal_diffuse, &tex_metal_specular);
Box metalDoorLeft(&tex_metal_diffuse, &tex_metal_specular);
Box metalDoorRight(&tex_metal_diffuse, &tex_metal_specular);

// win/lose condition boxes
Box loseBox(&tex_lose, &tex_lose_specular);
Box winBox(&tex_win, &tex_win_specular);

// Which door is next
enum DoorStage {
	WOOD = 0,
	BRICK = 1,
	MARBLE = 2,
	METAL = 3,
	FINISH = 4
};
DoorStage doorStage;

Box* colliders[4];
int collidersLen = 4;

// Door animations
float doorAnim[4];

enum GameStage {
	PLAYING,
	WON,
	LOST
};
GameStage gameStage;

void initialiseState();
void doorAnimationStep(DoorStage stage);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void computeBounds(float&, float&, float&, float&, float, glm::vec3);
bool checkBoxRange(Box, float);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow *window);
unsigned int loadTexture(char const * path);
void checkWinCond();

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
		std::cout << "Failed to create GLFW window:" << std::endl;
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


	// Initialise state
	initialiseState();
	//gameStage = LOST;

	// build and compile our shader program
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
	//tex_marble2_diffuse = loadTexture(FileSystem::getPath("resources/textures/marble2.jpg").c_str());
	//tex_marble2_specular = loadTexture(FileSystem::getPath("resources/textures/marble2_specular.jpg").c_str());
	tex_street_diffuse = loadTexture(FileSystem::getPath("resources/textures/street.png").c_str());
	tex_street_specular = loadTexture(FileSystem::getPath("resources/textures/street_specular.png").c_str());
	tex_grass_diffuse = loadTexture(FileSystem::getPath("resources/textures/grass.jpg").c_str());
	tex_grass_specular = loadTexture(FileSystem::getPath("resources/textures/grass_specular.jpg").c_str());
	tex_marble_diffuse = loadTexture(FileSystem::getPath("resources/textures/marble.jpg").c_str());
	//tex_marble_specular = loadTexture(FileSystem::getPath("resources/textures/marble.jpg").c_str());
	tex_marble_specular = loadTexture(FileSystem::getPath("resources/textures/marble_specular.jpg").c_str());
	//tex_curtin_diffuse = loadTexture(FileSystem::getPath("resources/textures/curtin.jpg").c_str());
	//tex_curtin_specular = loadTexture(FileSystem::getPath("resources/textures/curtin_specular.jpg").c_str());
	tex_red_dark_diffuse = loadTexture(FileSystem::getPath("resources/textures/red_dark.jpg").c_str());
	tex_red_dark_specular = loadTexture(FileSystem::getPath("resources/textures/red_dark_specular.jpg").c_str());
	tex_red_bright_diffuse = loadTexture(FileSystem::getPath("resources/textures/red_bright.jpg").c_str());
	tex_red_bright_specular = loadTexture(FileSystem::getPath("resources/textures/red_bright_specular.jpg").c_str());
	//tex_red_diffuse = loadTexture(FileSystem::getPath("resources/textures/red.jpg").c_str());
	//tex_red_specular = loadTexture(FileSystem::getPath("resources/textures/red_specular.jpg").c_str());
	tex_green_diffuse = loadTexture(FileSystem::getPath("resources/textures/green.jpg").c_str());
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
	tex_night_sky = loadTexture(FileSystem::getPath("resources/textures/night_sky.jpg").c_str());
	tex_win = loadTexture(FileSystem::getPath("resources/textures/win.png").c_str());
	tex_win_specular = loadTexture(FileSystem::getPath("resources/textures/win_specular.png").c_str());
	tex_lose = loadTexture(FileSystem::getPath("resources/textures/lose.png").c_str());
	tex_lose_specular = loadTexture(FileSystem::getPath("resources/textures/lose_specular.png").c_str());

	//shader configuration -----------------------------------------------------
	lighting_shader.use();
	lighting_shader.setInt("material.diffuse", 0);
	lighting_shader.setInt("material.specular", 1);


	// set static transformations ----------------------------------------------

	// Walls
	{
		float length = 34.0f;
		float zOffset = -8.0f;
		float dimen = 3.0f;
		float width = 0.0001f;

		// right wall
		rightWall.translate = glm::vec3(dimen / 2, dimen / 2, zOffset);
		rightWall.scale = glm::vec3(width, dimen, length);

		// left wall
		leftWall.translate = glm::vec3(-dimen / 2, dimen / 2, zOffset);
		leftWall.scale = glm::vec3(width, dimen, length);

		// floor
		floorBox.translate = glm::vec3(0.0f, 0.0f, zOffset);
		floorBox.scale = glm::vec3(dimen, width, length);

		// roof
		roofBox.translate = glm::vec3(0.0f, dimen, zOffset);
		roofBox.scale = glm::vec3(dimen, width, length);

		// back wall
		backWall.translate = glm::vec3(0.0f, dimen / 2, length / 2 + zOffset);
		backWall.scale = glm::vec3(dimen, dimen, width);

		// front wall
		frontWall.translate = glm::vec3(0.0f, dimen / 2, -length / 2 + zOffset);
		frontWall.scale = glm::vec3(dimen, dimen, width);

		// win square
		winSquare.translate = glm::vec3(0.0f, 0.025f, -length / 2 + zOffset + dimen);
		winSquare.scale = glm::vec3(dimen / 1.5f, 0.05f, dimen / 1.5f);
	}

	

	// bad guy
	{
		badGuy.MovementSpeed = SPEED / 3.0f;

		badGuyBox.scale = glm::vec3(0.08f, 0.08f, 0.16f);
		//badGuyBox.translate = glm::vec3(0.0f, 0.6f, 5.0f);

		badGuyBoxBack.scale = glm::vec3(0.18f);

		badGuyVert.scale = glm::vec3(0.05f, 2.0f, 0.05f);
	}

	// doors
	float doorZScale = 0.8f;
	woodDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);
	brickDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);
	marbleDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);
	// metal door
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
			buttons[i]->xAngle = 90.0f;
		}
	}

	{
		Box* buttons[] = { &button4_1, &button4_2, &button4_3 };
		for (int i = 0; i < 3; i++) {
			buttons[i]->scale = glm::vec3(buttonDimenScale);
			buttons[i]->xAngle = 90.0f;
		}
	}

	// win/lose screens
	loseBox.scale = glm::vec3(0.5f, 0.5f, 0.5f);
	winBox.scale = glm::vec3(0.5f, 0.5f, 0.5f);


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
		input_delay = max(0, input_delay - 1);

		// input
		// -----
		process_input(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// activate shader
		if (lamp_carrying) {
			float minX, maxX, minZ, maxZ, pad = HITBOX_PAD / 4;
			light_pos = camera.Position;
			light_pos.x += 0.5f * camera.Front.x + 0.1f * camera.Right.x;
			light_pos.y += 0.3f * camera.Front.y - 0.15f;
			light_pos.z += 0.5f * camera.Front.z + 0.1f * camera.Right.z;
			computeBounds(minX, maxX, minZ, maxZ, pad, light_pos);
			light_pos.x = med(minX, maxX, light_pos.x);
			light_pos.z = med(minZ, maxZ, light_pos.z);
		} else {
			double time = glfwGetTime();
			light_pos = glm::vec3(0.0f, 0.3f + sin(glm::radians(time * 180)) / 7.0f, -0.2f);
			lamp_carrying = glm::length(camera.Position - light_pos) < BTN_RANGE;
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
				glm::radians(camera.Fov),
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

		// render static objects -----------------------------------------------
		floorBox.render(lighting_shader, VAO_box);
		roofBox.render(lighting_shader, VAO_box);
		rightWall.render(lighting_shader, VAO_box);
		leftWall.render(lighting_shader, VAO_box);
		backWall.render(lighting_shader, VAO_box);
		frontWall.render(lighting_shader, VAO_box);
		winSquare.render(lighting_shader, VAO_box);


		// doors ---------------------------------------------------------------

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
			brickDoor.zAngle = anim * -90.0f;
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
				marbleDoor.zAngle = localAnim * 720.0f;
				marbleDoor.translate.z = initialTranslate.z + initialTranslate.z * 1.5f * sin(glm::radians(localAnim * 45.0f));
			}
		}
		doorAnimationStep(MARBLE);
		marbleDoor.render(lighting_shader, VAO_box);


		// Metal door
		{
			float anim = doorAnim[DoorStage::METAL];
			if (anim < 0.4f) {
				float localAnim = anim / 4.0f * 10.0f;
				metalDoorLeft.translate.y = 1.5f - 3.0f * sin(glm::radians(localAnim * 90.0f));
				metalDoorRight.translate.y = 1.5f + 3.0f * sin(glm::radians(localAnim * 90.0f));
			} else {
				float localAnim = (anim - 0.4f) / 6.0f * 10.0f;
				metalDoorBot.translate.x = -3.6f * sin(glm::radians(localAnim * 90.0f));
				metalDoorTop.translate.x = 3.6f * sin(glm::radians(localAnim * 90.0f));
			}

			if (anim == 1.0f) {
				// Move things that should not be visible out
				metalDoorBot.translate.y = 5.0f;
				metalDoorLeft.translate.x = -5.0f;
				metalDoorRight.translate.x = 5.0f;
			}

			doorAnimationStep(METAL);
			Box* metalDoors[] = {
				&metalDoorBot, &metalDoorTop, &metalDoorLeft, &metalDoorRight
			};
			for (int i = 0; i < 4; i++) {
				metalDoors[i]->render(lighting_shader, VAO_box);
			}
		}

		// buttons -------------------------------------------------------------

		// Button 1 (wood door)
		{
			float buttonXOffset;
			if (buttonPressed[WOOD]) {
				buttonXOffset = -0.04f;
			} else {
				buttonXOffset = 0.02f;
			}
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
				buttonXOffset = -0.04f;
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
				buttonXOffset = -0.04f;
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

		// Button 4's (metal door)
		{
			Box* button4s[] = { &button4_1, &button4_2, &button4_3 };
			float buttonXOffset;
			if (buttonPressed[METAL]) {
				buttonXOffset = -0.04f;
			} else {
				buttonXOffset = 0.02f;
			}
			float x = rightWall.getNegativeBounds().x - 0.02f;
			float y = camera.Position.y - 0.2f;
			float z = metalDoorBot.getPositiveBounds().z + 0.5f;
			for (int i = 0; i < 3; i++) {
				button4s[i]->translate = glm::vec3(x - buttonXOffset, y, z + i * 0.7f);
				button4s[i]->render(lighting_shader, VAO_box);
			}
		}


		// bad guy -------------------------------------------------------------
		if (gameStage != WON) {
			glm::vec3 direction = glm::normalize(camera.Position - badGuy.Position);
			badGuy.Front = direction;

			badGuy.CamHeight = cam_height + sin(glfwGetTime() * 2.5f) / 7.0f;

			badGuyBox.translate = badGuy.Position;
			badGuyBox.yAngle = glm::degrees(atan(direction.x / direction.z));
			badGuyBox.xAngle = -glm::degrees(atan(direction.y / direction.z));

			badGuyBoxBack.translate = badGuyBox.translate;
			badGuyBoxBack.translate.x -= direction.x * 0.08f;
			badGuyBoxBack.translate.z -= direction.z * 0.08f;
			badGuyBoxBack.yAngle = glm::degrees(atan(direction.x / direction.z));
			badGuyBoxBack.xAngle = -glm::degrees(atan(direction.y / direction.z));

			badGuyVert.scale.y = badGuy.CamHeight;
			badGuyVert.translate = badGuyBox.translate;
			badGuyVert.translate.x -= direction.x * 0.08f;
			badGuyVert.translate.y /= 2;
			badGuyVert.translate.z -= direction.z * 0.08f;
			badGuyVert.yAngle = glm::degrees(atan(direction.x / direction.z));

			badGuyBox.render(lighting_shader, VAO_box);
			badGuyBoxBack.render(lighting_shader, VAO_box);
			badGuyVert.render(lighting_shader, VAO_box);

			float minX, maxX, minZ, maxZ;
			float pad = (HITBOX_PAD / 8) + (badGuyBox.scale.x / 2) * 0;
			computeBounds(minX, maxX, minZ, maxZ, pad, badGuy.Position);
			badGuy.ProcessKeyboard(FORWARD, delta_time, minX, maxX, minZ, maxZ);
		
			
		}

		
		if (gameStage == PLAYING) {
			// Check win/lose conditions
			if (checkBoxRange(badGuyBox, BTN_RANGE / 4)) {
				gameStage = LOST;

			} else {
				glm::vec3 minBound = winSquare.getNegativeBounds();
				glm::vec3 maxBound = winSquare.getPositiveBounds();

				if (camera.Position.x > minBound.x &&
					camera.Position.x < maxBound.x &&
					camera.Position.z > minBound.z &&
					camera.Position.z < maxBound.z
				) {
					gameStage = WON;
				}
			}

		} else if (gameStage == LOST) {
			// death screen
			camera.CamHeight = 0.3f;
			camera.Pitch = 0.0f;
			loseBox.translate = camera.Position;
			loseBox.translate.x += 1.0f * camera.Front.x;
			loseBox.translate.y += camera.Front.y;
			loseBox.translate.z += 1.0f * camera.Front.z;
			loseBox.yAngle = -camera.Yaw;
			loseBox.xAngle = 90.0f;
			loseBox.render(lighting_shader, VAO_box);

		} else if (gameStage == WON) {
			// win screen
			winBox.translate = camera.Position;
			winBox.translate.x += 1.0f * camera.Front.x;
			winBox.translate.y += camera.Front.y;
			winBox.translate.z += 1.0f * camera.Front.z;
			winBox.yAngle = -camera.Yaw;
			winBox.xAngle = 90.0f;
			winBox.render(lighting_shader, VAO_box);
		}


		// Draw the light source -----------------------------------------------
		{
			float lampYAngle;
			if (lamp_carrying) {
				lampYAngle = -camera.Yaw;
			} else {
				lampYAngle = sin(glfwGetTime()) * 90.0f;
			}

			Box lampShaft(&tex_wood_diffuse, &tex_wood_specular);
			lampShaft.translate = glm::vec3(light_pos.x, light_pos.y - 0.045f, light_pos.z);
			lampShaft.scale = glm::vec3(0.013f, 0.08f, 0.013f);
			lampShaft.yAngle = lampYAngle;
			lampShaft.render(lighting_shader, VAO_box);

			Box lampDetailTop(&tex_wood_diffuse, &tex_wood_specular);
			lampDetailTop.translate = glm::vec3(light_pos.x, light_pos.y - 0.01f, light_pos.z);
			lampDetailTop.scale = glm::vec3(0.02f, 0.01, 0.02f);
			lampDetailTop.yAngle = lampYAngle;
			lampDetailTop.render(lighting_shader, VAO_box);

			Box lampDetailBot(&tex_wood_diffuse, &tex_wood_specular);
			lampDetailBot.translate = glm::vec3(light_pos.x, light_pos.y - 0.08f, light_pos.z);
			lampDetailBot.scale = glm::vec3(0.02f, 0.025, 0.02f);
			lampDetailBot.yAngle = lampYAngle;
			lampDetailBot.render(lighting_shader, VAO_box);

			lamp_shader.use();
			lamp_shader.setFloat("intensity", 1.2);
			lamp_shader.setMat4("projection", projection);
			lamp_shader.setMat4("view", view);

			Box lampLight(&tex_red_bright_diffuse, &tex_red_specular);
			lampLight.translate = light_pos;
			lampLight.scale = glm::vec3(0.01f);
			lampLight.yAngle = lampYAngle;
			lampLight.render(lamp_shader, VAO_light);
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// de allocate resources
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

	if (gameStage == PLAYING) {
		// Sprinting animation
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			sprintAnim = min(1.0f, sprintAnim + delta_time * 3);
		} else {
			sprintAnim = max(0.0f, sprintAnim - delta_time * 3);
		}
		camera.MovementSpeed = SPEED + SPEED * sin(glm::radians(sprintAnim * 90.0f));
		camera.Fov = 10.0f * sin(glm::radians(sprintAnim * 90.0f)) + fov;

		bool w, s, a, d;
		w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

		if (w || s || a || d) {
			// Bounds
			float minX, maxX, minZ, maxZ;
			computeBounds(minX, maxX, minZ, maxZ, HITBOX_PAD, camera.Position);

			if (w && !s) {
				camera.ProcessKeyboard(FORWARD, delta_time, minX, maxX, minZ, maxZ);
			} else if (s && !w) {
				camera.ProcessKeyboard(BACKWARD, delta_time, minX, maxX, minZ, maxZ);
			}

			if (a && !d) {
				camera.ProcessKeyboard(LEFT, delta_time, minX, maxX, minZ, maxZ);
			} else if (d && !a) {
				camera.ProcessKeyboard(RIGHT, delta_time, minX, maxX, minZ, maxZ);
			}
		}

		// Buttons
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && input_delay == 0) {
			if (checkBoxRange(button1Box, BTN_RANGE)) {
				//toggle button 1 (wood door)
				input_delay = INPUT_MAX;
				buttonPressed[WOOD] = true;
				if (doorStage == WOOD) {
					doorStage = BRICK;
				}
			} else if (checkBoxRange(button2_brick, BTN_RANGE)) {
				//toggle button 2 (brick door)
				input_delay = INPUT_MAX;
				buttonPressed[BRICK] = true;
				if (doorStage == BRICK) {
					doorStage = MARBLE;
				}
			} else if (checkBoxRange(button2_metal, BTN_RANGE)) {
				// false button2 (metal)
				input_delay = INPUT_MAX;
				button2_metal.diffuseTex = &tex_red_dark_diffuse;
				button2_metal.specularTex = &tex_red_dark_specular;
			} else if (checkBoxRange(button2_wood, BTN_RANGE)) {
				// false button2 (wood)
				input_delay = INPUT_MAX;
				button2_wood.diffuseTex = &tex_red_dark_diffuse;
				button2_wood.specularTex = &tex_red_dark_specular;
			} else if (checkBoxRange(button3_3, BTN_RANGE)) {
				// toggle button 3 (marble graffiti door)
				input_delay = INPUT_MAX;
				buttonPressed[MARBLE] = true;
				if (doorStage == MARBLE) {
					doorStage = METAL;
				}
			} else if (checkBoxRange(button3_2, BTN_RANGE)) {
				// false button3 (1+2=2)
				input_delay = INPUT_MAX;
				button3_2.diffuseTex = &tex_red_dark_diffuse;
				button3_2.specularTex = &tex_red_dark_specular;
			} else if (checkBoxRange(button3_4, BTN_RANGE)) {
				// false button3 (1+2=4)
				input_delay = INPUT_MAX;
				button3_4.diffuseTex = &tex_red_dark_diffuse;
				button3_4.specularTex = &tex_red_dark_specular;
			} else if (
				checkBoxRange(button4_1, BTN_RANGE)
				|| checkBoxRange(button4_2, BTN_RANGE)
				|| checkBoxRange(button4_3, BTN_RANGE)
			) {
				// toggle button 4's (all of them) (metal door)
				input_delay = INPUT_MAX;
				buttonPressed[METAL] = true;
				if (doorStage == METAL) {
					doorStage = FINISH;
				}
			}
		}
	} else {
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && input_delay == 0) {
			// Reset game
			initialiseState();
		}
	}


	// toggle projection mode
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && input_delay == 0) {
		input_delay = INPUT_MAX;
		PERSPECTIVE_PROJECTION = !PERSPECTIVE_PROJECTION;
	}

	// toggle extra brightness
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && input_delay == 0) {
		input_delay = INPUT_MAX;
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

void initialiseState() {
	// camera
	cam_height = 0.8f;
	camera.Position = glm::vec3(0.0f, cam_height, 3.0f);
	camera.CamHeight = cam_height;
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;
	firstMouse = true;
	fov = 60.0f;
	camera.Yaw = -90.0f;
	camera.Pitch = 0.0f;
	camera.updateCameraVectors();

	badGuy.CamHeight = cam_height;
	badGuy.Position = glm::vec3(0.0f, cam_height, 9.0f);

	sprintAnim = 0.0f;

	input_delay = 0;

	// Light
	light_radius = 1.2f;
	lamp_carrying = false;
	light_pos = LIGHT_INITIAL_POS;

	//Toggle (Animation or states)
	for (int i = 0; i < 4; i++) {
		buttonPressed[i] = false;
	}

	PERSPECTIVE_PROJECTION = true;

	EXTRA_BRIGHT = false;

	// Which door is next
	doorStage = WOOD;

	// Door animations
	for (int i = 0; i < 4; i++) {
		doorAnim[i] = 0.0f;
	}

	// Colliders
	colliders[0] = &woodDoor;
	colliders[1] = &brickDoor;
	colliders[2] = &marbleDoor;
	colliders[3] = &metalDoorBot;

	gameStage = PLAYING;
}

void computeBounds(float& minX, float& maxX, float& minZ, float& maxZ, float pad, glm::vec3 pos) {
	// Start with bounds of static walls
	minX = leftWall.getPositiveBounds().x + pad;
	maxX = rightWall.getNegativeBounds().x - pad;
	minZ = floorBox.getNegativeBounds().z + pad;
	maxZ = floorBox.getPositiveBounds().z - pad;

	for (int i = 0; i < collidersLen; i++) {
		glm::vec3 wallMin = colliders[i]->getNegativeBounds();
		glm::vec3 wallMax = colliders[i]->getPositiveBounds();



		if (wallMin.x < pos.x && pos.x < wallMax.x) {
			// Make sure camera is within the x range of the wall

			if (pos.z > wallMax.z && pos.y > wallMin.y) {
				// Try to walk through from +ve z side
				minZ = max(minZ, wallMax.z + pad);
			}
			if (pos.z < wallMin.z && pos.y > wallMin.y) {
				// Try to walk through from -ve z side
				maxZ = min(maxZ, wallMin.z - pad);
			}
		}

		if (wallMin.z < pos.z && pos.z < wallMax.z) {
			// Player has a wall to their left/right (z axis)

			if (pos.x > wallMax.x && pos.y > wallMin.y) {
				// Try to walk through from +ve z side
				minX = max(minX, wallMax.x + pad);
			}
			if (pos.z < wallMin.x && pos.y > wallMin.y) {
				// Try to walk through from -ve z side
				maxX = min(maxX, wallMin.x - pad);
			}
		}
	}
}

bool checkBoxRange(Box box, float range = BTN_RANGE) {
	return glm::length(camera.Position - box.translate) <= range;
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

	if (gameStage == PLAYING) {

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
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		if (gameStage == PLAYING) {
		camera.ProcessMouseScroll(yoffset);
		fov = camera.Fov;
	}
}
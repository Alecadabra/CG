#include "assignment.h"

// Textures --------------------------------------------------------------------

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

// Constant settings -----------------------------------------------------------

// Window size in pixels
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Clipping planes
const float NEAR_CLIP = 0.1f;
const float FAR_CLIP = 50.0f;

// Padding - minimum distance the camera can be from a collidable surface
const float HITBOX_PAD = 0.25f;

// Minimum range the camera has to be to push a physical button
const float BTN_RANGE = 0.8f;

// Delay after pushing a button until another can be pressed
const int INPUT_MAX = 20;

// Initial position of the light source
const glm::vec3 LIGHT_INITIAL_POS = glm::vec3(0.0f, 0.4f, -0.5f);

// Height of the camera
const float CAM_HEIGHT = 0.8f;

// Game state ------------------------------------------------------------------

// Camera
Camera camera(glm::vec3(0.0f));
float lastX;
float lastY;
bool firstMouse;
// Camera zoom, independent of camera object zoom because of sprint animation
float fov;

// Bad guy
Camera bad_guy(glm::vec3(0.0f));

// Sprint animation - 0 to 1 for a smooth sprint effect
float sprint_anim;

// Timing
// Time between current frame and last frame
float delta_time = 0.0f; 
float last_frame = 0.0f;

// Countdown until a button can be pressed
int input_delay;

// Light
float light_radius;
// Whether the player is holding the lamp
bool lamp_carrying;
glm::vec3 light_pos;

// State of buttons
bool button_pressed[4];
int button_pressed_len = 4;
// Other 'wrong' buttons
bool button_2_metal_pressed;
bool button_2_wood_pressed;
bool button_3_2_pressed;
bool button_3_4_pressed;

// True for perspective projection, false for othographic (parallel) projection
bool perspective_proj;

// If true, ambient light is increased dramatically
bool extra_bright;

// Which door is next
enum DoorStage {
	WOOD = 0,
	BRICK = 1,
	MARBLE = 2,
	METAL = 3,
	FINISH = 4
};
DoorStage door_stage;

Box* colliders[4];
int colliders_len = 4;

// Door animations
float door_anim[4];

enum GameStage {
	PLAYING,
	WON,
	LOST
};
GameStage game_stage;

// Boxes -----------------------------------------------------------------------

// Staticlly transformed boxes
Box floor_box(&tex_marble_diffuse, &tex_marble_specular, true);
Box roof_box(&tex_marble_diffuse, &tex_marble_specular, true);
Box right_wall(&tex_marble_diffuse, &tex_marble_specular, true);
Box left_wal(&tex_marble_diffuse, &tex_marble_specular, true);
Box back_wall(&tex_marble_diffuse, &tex_marble_specular, true);
Box front_wall(&tex_marble_diffuse, &tex_marble_specular, true);
Box win_square(&tex_green_diffuse, &tex_metal_specular, true);

// Bad guy
Box bad_guy_box_eye(&tex_red_bright_diffuse, &tex_marble_specular);
Box bad_guy_box_back(&tex_marble2_diffuse, &tex_marble2_specular);
Box bad_guy_box_stem(&tex_marble2_diffuse, &tex_marble2_specular);

// Button boxes
Box button_1_box(&tex_green_diffuse, &tex_metal_specular);
Box button_2_box_brick(&tex_brickwall_diffuse, &tex_brickwall_specular);
Box button_2_box_metal(&tex_metal_diffuse, &tex_metal_specular);
Box button_2_box_wood(&tex_wood_diffuse, &tex_wood_specular);
Box button_3_box_2(&tex_graffiti_2_diffuse, &tex_graffiti_2_specular);
Box button_3_box_3(&tex_graffiti_3_diffuse, &tex_graffiti_3_specular);
Box button_3_box_4(&tex_graffiti_4_diffuse, &tex_graffiti_4_specular);
Box button_4_box_1(&tex_grass_diffuse, &tex_grass_specular);
Box button_4_box_2(&tex_curtin_diffuse, &tex_curtin_specular);
Box button_4_box_3(&tex_street_diffuse, &tex_street_specular);

// Door boxes
Box woodDoor(&tex_wood_diffuse, &tex_wood_specular);
Box brick_door(&tex_brickwall_diffuse, &tex_brickwall_specular);
Box marble_door(&tex_marble2_graffiti_diffuse, &tex_marble2_graffiti_specular);
Box metal_door_bottom(&tex_metal_diffuse, &tex_metal_specular);
Box metal_door_top(&tex_metal_diffuse, &tex_metal_specular);
Box metal_door_left(&tex_metal_diffuse, &tex_metal_specular);
Box metal_door_right(&tex_metal_diffuse, &tex_metal_specular);

// Win/lose condition boxes
Box lose_box(&tex_lose, &tex_lose_specular);
Box win_box(&tex_win, &tex_win_specular);

// Function forward declarations -----------------------------------------------

void initialiseState();
void doorAnimationStep(DoorStage stage);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void computeBounds(float&, float&, float&, float&, float, glm::vec3);
bool checkBoxRange(Box, float);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);

// Program ---------------------------------------------------------------------

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
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
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
	tex_marble2_diffuse = loadTexture(FileSystem::getPath("resources/textures/marble2.jpg").c_str());
	tex_marble2_specular = loadTexture(FileSystem::getPath("resources/textures/marble2_specular.jpg").c_str());
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
		float width = 0.001f;

		// right wall
		right_wall.translate = glm::vec3(dimen / 2, dimen / 2, zOffset);
		right_wall.scale = glm::vec3(width, dimen, length);

		// left wall
		left_wal.translate = glm::vec3(-dimen / 2, dimen / 2, zOffset);
		left_wal.scale = glm::vec3(width, dimen, length);

		// floor
		floor_box.translate = glm::vec3(0.0f, 0.0f, zOffset);
		floor_box.scale = glm::vec3(dimen, width, length);

		// roof
		roof_box.translate = glm::vec3(0.0f, dimen, zOffset);
		roof_box.scale = glm::vec3(dimen, width, length);

		// back wall
		back_wall.translate = glm::vec3(0.0f, dimen / 2, length / 2 + zOffset);
		back_wall.scale = glm::vec3(dimen, dimen, width);

		// front wall
		front_wall.translate = glm::vec3(0.0f, dimen / 2, -length / 2 + zOffset);
		front_wall.scale = glm::vec3(dimen, dimen, width);

		// win square
		win_square.translate = glm::vec3(0.0f, 0.025f, -length / 2 + zOffset + dimen);
		win_square.scale = glm::vec3(dimen / 1.5f, 0.05f, dimen / 1.5f);
	}

	

	// bad guy
	{
		bad_guy.MovementSpeed = SPEED / 3.0f;

		bad_guy_box_eye.scale = glm::vec3(0.08f, 0.08f, 0.16f);
		//badGuyBox.translate = glm::vec3(0.0f, 0.6f, 5.0f);

		bad_guy_box_back.scale = glm::vec3(0.18f);

		bad_guy_box_stem.scale = glm::vec3(0.05f, 2.0f, 0.05f);
	}

	// doors
	float doorZScale = 0.8f;
	woodDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);
	brick_door.scale = glm::vec3(3.0f, 3.0f, doorZScale);
	marble_door.scale = glm::vec3(3.0f, 3.0f, doorZScale);
	// metal door
	glm::vec3 metalDoorInitialScale[4];
	glm::vec3 metalDoorInitialTranslate[4];
	{
		float holeSize = 1.2f;
		float width = (3.0f - holeSize) / 2;
		Box* metalDoors[] = {
			&metal_door_bottom, &metal_door_top, &metal_door_left, &metal_door_right
		};
		for (int i = 0; i < 4; i++) {
			metalDoors[i]->scale = glm::vec3(3.0f, 3.0f, doorZScale);
			metalDoors[i]->translate = glm::vec3(0.0f, 1.5f, -20.0f);
		}
		// Make size correct
		metal_door_top.scale.y = width;
		metal_door_bottom.scale.y = width;
		metal_door_left.scale.x = width;
		metal_door_right.scale.x = width;

		// Move to correct positions
		metal_door_top.translate.y = 3.0f - width / 2;
		metal_door_bottom.translate.y = width / 2;
		metal_door_left.translate.x = -1.5f + width / 2;
		metal_door_right.translate.x = 1.5f - width / 2;

		// Make l/r skinny on z
		metal_door_left.scale.z /= 1.5f;
		metal_door_right.scale.z /= 1.5f;

		// Make l/r short
		metal_door_left.scale.y = holeSize;
		metal_door_right.scale.y = holeSize;

		for (int i = 0; i < 4; i++) {
			metalDoorInitialScale[i] = metalDoors[i]->scale;
			metalDoorInitialTranslate[i] = metalDoors[i]->translate;
		}
	}

	// buttons
	float buttonDimenScale = 0.12f;
	button_1_box.scale = glm::vec3(buttonDimenScale);
	
	button_2_box_brick.scale = glm::vec3(buttonDimenScale);
	button_2_box_metal.scale = glm::vec3(buttonDimenScale);
	button_2_box_wood.scale = glm::vec3(buttonDimenScale);

	{
		Box* buttons[] = { &button_3_box_2, &button_3_box_3, &button_3_box_4 };
		for (int i = 0; i < 3; i++) {
			buttons[i]->scale = glm::vec3(buttonDimenScale);
			buttons[i]->xAngle = 90.0f;
		}
	}

	{
		Box* buttons[] = { &button_4_box_1, &button_4_box_2, &button_4_box_3 };
		for (int i = 0; i < 3; i++) {
			buttons[i]->scale = glm::vec3(buttonDimenScale);
			buttons[i]->xAngle = 90.0f;
		}
	}

	// win/lose screens
	lose_box.scale = glm::vec3(0.5f, 0.5f, 0.5f);
	win_box.scale = glm::vec3(0.5f, 0.5f, 0.5f);


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
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// activate shader
		if (lamp_carrying) {
			float minX, maxX, minZ, maxZ, pad = HITBOX_PAD / 2;
			light_pos = camera.Position;
			computeBounds(minX, maxX, minZ, maxZ, pad, light_pos);
			light_pos.x += 0.5f * camera.Front.x + 0.1f * camera.Right.x;
			light_pos.y += 0.3f * camera.Front.y - 0.15f;
			light_pos.z += 0.5f * camera.Front.z + 0.1f * camera.Right.z;
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
		if (extra_bright) {
			lighting_shader.setVec3("light.ambient", 0.6f, 0.6f, 0.6f);
		} else {
			lighting_shader.setVec3("light.ambient", 0.08f, 0.08f, 0.08f);
		}

		float brightness = abs(cos(glfwGetTime()) / 2 ) + 1.2f;
		lighting_shader.setVec3("light.diffuse", glm::vec3(brightness));
		lighting_shader.setVec3("light.specular", glm::vec3(brightness));

		// material properties
        lighting_shader.setFloat("material.shininess", 15.0f);
		lighting_shader.setFloat("radius", light_radius);

		// pass projection matrix to shader
		glm::mat4 projection;
		if (perspective_proj) {
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

		floor_box.render(lighting_shader, VAO_box);
		roof_box.render(lighting_shader, VAO_box);
		right_wall.render(lighting_shader, VAO_box);
		left_wal.render(lighting_shader, VAO_box);
		back_wall.render(lighting_shader, VAO_box);
		front_wall.render(lighting_shader, VAO_box);
		win_square.render(lighting_shader, VAO_box);


		// Doors ---------------------------------------------------------------

		// Wood door
		{
			float yVal = woodDoor.scale.y / 2 + sin(glm::radians(door_anim[DoorStage::WOOD] * 90.0f)) * 2.0f;
			woodDoor.translate = glm::vec3(0, yVal, -5.0f);

			doorAnimationStep(WOOD);
			woodDoor.render(lighting_shader, VAO_box);
		}

		// Brick door
		{
			float anim = door_anim[DoorStage::BRICK];
			float x = (brick_door.scale.x - 0.3f) * anim * anim;
			float y = brick_door.scale.y / 2 + sin(glm::radians(anim * anim * 180.0f));
			brick_door.translate = glm::vec3(x, y, -10.0f);
			brick_door.zAngle = anim * -90.0f;
			float deltaHeight = sin(glm::radians(anim * anim * 180)) * 2;
			brick_door.scale.x = woodDoor.scale.x - deltaHeight;
			brick_door.scale.y = woodDoor.scale.y - deltaHeight;

			doorAnimationStep(BRICK);
			brick_door.render(lighting_shader, VAO_box);
		}

		// Marble graffiti door
		{
			float anim = door_anim[DoorStage::MARBLE];
			glm::vec3 initialScale = glm::vec3(3.0f, 3.0f, doorZScale);
			glm::vec3 initialTranslate = glm::vec3(0.0f, initialScale.y / 2, -15.0f);
			
			if (anim == 0.0f) {
				marble_door.zAngle = 0.0f;
				marble_door.translate = initialTranslate;
				marble_door.scale = initialScale;
			} else if (anim < 0.3f) {
				float localAnim = anim / 3.0f * 10.0f;
				marble_door.scale.x = initialScale.x - (initialScale.x - 1.0f) * sin(glm::radians(localAnim * 90.0f));
			} else if (anim < 0.6f) {
				float localAnim = (anim - 0.3f) / 3.0f * 10.0f;
				marble_door.scale.y = initialScale.y - (initialScale.y - 1.0f) * sin(glm::radians(localAnim * 90.0f));
			} else {
				float localAnim = (anim - 0.6f) / 3.0f * 10.0f;
				marble_door.zAngle = localAnim * 720.0f;
				marble_door.translate.z = initialTranslate.z + initialTranslate.z * 1.5f * sin(glm::radians(localAnim * 45.0f));
			}

			doorAnimationStep(MARBLE);
			marble_door.render(lighting_shader, VAO_box);
		}


		// Metal door
		{
			Box* metalDoors[] = {
				&metal_door_bottom, &metal_door_top, &metal_door_left, &metal_door_right
			};
			float anim = door_anim[DoorStage::METAL];
			if (anim == 0.0f) {
				for (int i = 0; i < 4; i++) {
					metalDoors[i]->scale = metalDoorInitialScale[i];
					metalDoors[i]->translate = metalDoorInitialTranslate[i];
				}
			} else if (anim < 0.4f) {
				float localAnim = anim / 4.0f * 10.0f;
				metal_door_left.translate.y = 1.5f - 3.0f * sin(glm::radians(localAnim * 90.0f));
				metal_door_right.translate.y = 1.5f + 3.0f * sin(glm::radians(localAnim * 90.0f));
			} else {
				float localAnim = (anim - 0.4f) / 6.0f * 10.0f;
				metal_door_bottom.translate.x = -3.6f * sin(glm::radians(localAnim * 90.0f));
				metal_door_top.translate.x = 3.6f * sin(glm::radians(localAnim * 90.0f));
			}

			if (anim == 1.0f) {
				// Move things that should not be visible out
				metal_door_bottom.translate.y = 5.0f;
				metal_door_left.translate.x = -5.0f;
				metal_door_right.translate.x = 5.0f;
			}

			doorAnimationStep(METAL);
			for (int i = 0; i < 4; i++) {
				metalDoors[i]->render(lighting_shader, VAO_box);
			}
		}

		// buttons -------------------------------------------------------------

		// Button 1 (wood door)
		{
			float buttonXOffset;
			if (button_pressed[WOOD]) {
				buttonXOffset = -0.04f;
			} else {
				buttonXOffset = 0.02f;
			}
			float x = right_wall.getNegativeBounds().x - buttonXOffset;
			float y = camera.Position.y - 0.2f;
			float z = woodDoor.getPositiveBounds().z + 0.5f;
			button_1_box.translate = glm::vec3(x, y, z);
		}
		button_1_box.render(lighting_shader, VAO_box);

		// Button 2's (brick door)
		{
			Box* button2s[] = { &button_2_box_brick, &button_2_box_metal, &button_2_box_wood };
			float buttonXOffset;
			if (button_pressed[BRICK]) {
				buttonXOffset = -0.04f;
			} else {
				buttonXOffset = 0.02f;
			}

			if (button_2_metal_pressed) {
				button_2_box_metal.diffuseTex = &tex_red_dark_diffuse;
				button_2_box_metal.specularTex = &tex_red_dark_specular;
			} else {
				button_2_box_metal.diffuseTex = &tex_metal_diffuse;
				button_2_box_metal.specularTex = &tex_metal_specular;
			}

			if (button_2_wood_pressed) {
				button_2_box_wood.diffuseTex = &tex_red_dark_diffuse;
				button_2_box_wood.specularTex = &tex_red_dark_specular;
			} else {
				button_2_box_wood.diffuseTex = &tex_wood_diffuse;
				button_2_box_wood.specularTex = &tex_wood_specular;
			}

			float x = right_wall.getNegativeBounds().x - 0.02f;
			float y = camera.Position.y - 0.2f;
			float z = brick_door.getPositiveBounds().z + 0.5f;
			for (int i = 1; i < 3; i++) {
				button2s[i]->translate = glm::vec3(x, y, z + i * 0.7f);
			}
			button_2_box_brick.translate = glm::vec3(x - buttonXOffset, y, z);
			for (int i = 0; i < 3; i++) {
				button2s[i]->render(lighting_shader, VAO_box);
			}
		}

		// Button 3's (marble graffiti door)
		{
			Box initialDoor(&tex_brickwall_diffuse, &tex_brickwall_specular);
			initialDoor.scale = glm::vec3(3.0f, 3.0f, doorZScale);
			initialDoor.translate = glm::vec3(0.0f, initialDoor.scale.y / 2, -15.0f);
			Box* button3s[] = { &button_3_box_2, &button_3_box_3, &button_3_box_4 };
			float buttonXOffset;

			if (button_pressed[MARBLE]) {
				buttonXOffset = -0.04f;
			} else {
				buttonXOffset = 0.02f;
			}

			if (button_3_2_pressed) {
				button_3_box_2.diffuseTex = &tex_red_dark_diffuse;
				button_3_box_2.specularTex = &tex_red_dark_specular;
			} else {
				button_3_box_2.diffuseTex = &tex_graffiti_2_diffuse;
				button_3_box_2.specularTex = &tex_graffiti_2_specular;
			}

			if (button_3_4_pressed) {
				button_3_box_4.diffuseTex = &tex_red_dark_diffuse;
				button_3_box_4.specularTex = &tex_red_dark_specular;
			} else {
				button_3_box_4.diffuseTex = &tex_graffiti_4_diffuse;
				button_3_box_4.specularTex = &tex_graffiti_4_specular;
			}

			float x = right_wall.getNegativeBounds().x - 0.02f;
			float y = camera.Position.y - 0.2f;
			float z = initialDoor.getPositiveBounds().z + 0.5f;
			for (int i = 0; i < 3; i++) {
				button3s[i]->translate = glm::vec3(x, y, z + i * 0.7f);
			}
			button_3_box_3.translate.x -= buttonXOffset;
			for (int i = 0; i < 3; i++) {
				button3s[i]->render(lighting_shader, VAO_box);
			}
		}

		// Button 4's (metal door)
		{
			Box* button4s[] = { &button_4_box_1, &button_4_box_2, &button_4_box_3 };
			float buttonXOffset;
			if (button_pressed[METAL]) {
				buttonXOffset = -0.04f;
			} else {
				buttonXOffset = 0.02f;
			}
			float x = right_wall.getNegativeBounds().x - 0.02f;
			float y = camera.Position.y - 0.2f;
			float z = metal_door_bottom.getPositiveBounds().z + 0.5f;
			for (int i = 0; i < 3; i++) {
				button4s[i]->translate = glm::vec3(x - buttonXOffset, y, z + i * 0.7f);
				button4s[i]->render(lighting_shader, VAO_box);
			}
		}


		// bad guy -------------------------------------------------------------
		if (game_stage != WON) {
			glm::vec3 direction = glm::normalize(camera.Position - bad_guy.Position);
			bad_guy.Front = direction;

			bad_guy.CamHeight = CAM_HEIGHT + sin(glfwGetTime() * 2.5f) / 7.0f;

			bad_guy_box_eye.translate = bad_guy.Position;
			bad_guy_box_eye.yAngle = glm::degrees(atan(direction.x / direction.z));
			bad_guy_box_eye.xAngle = -glm::degrees(atan(direction.y / direction.z));

			bad_guy_box_back.translate = bad_guy_box_eye.translate;
			bad_guy_box_back.translate.x -= direction.x * 0.08f;
			bad_guy_box_back.translate.z -= direction.z * 0.08f;
			bad_guy_box_back.yAngle = glm::degrees(atan(direction.x / direction.z));
			bad_guy_box_back.xAngle = -glm::degrees(atan(direction.y / direction.z));

			bad_guy_box_stem.scale.y = bad_guy.CamHeight;
			bad_guy_box_stem.translate = bad_guy_box_eye.translate;
			bad_guy_box_stem.translate.x -= direction.x * 0.08f;
			bad_guy_box_stem.translate.y /= 2;
			bad_guy_box_stem.translate.z -= direction.z * 0.08f;
			bad_guy_box_stem.yAngle = glm::degrees(atan(direction.x / direction.z));

			bad_guy_box_eye.render(lighting_shader, VAO_box);
			bad_guy_box_back.render(lighting_shader, VAO_box);
			bad_guy_box_stem.render(lighting_shader, VAO_box);

			float minX, maxX, minZ, maxZ;
			float pad = (HITBOX_PAD / 8) + (bad_guy_box_eye.scale.x / 2) * 0;
			computeBounds(minX, maxX, minZ, maxZ, pad, bad_guy.Position);
			bad_guy.ProcessKeyboard(FORWARD, delta_time, minX, maxX, minZ, maxZ);
		}

		
		if (game_stage == PLAYING) {
			// Check win/lose conditions
			if (checkBoxRange(bad_guy_box_eye, BTN_RANGE / 4)) {
				game_stage = LOST;

			} else {
				glm::vec3 minBound = win_square.getNegativeBounds();
				glm::vec3 maxBound = win_square.getPositiveBounds();

				if (camera.Position.x > minBound.x &&
					camera.Position.x < maxBound.x &&
					camera.Position.z > minBound.z &&
					camera.Position.z < maxBound.z
				) {
					game_stage = WON;
				}
			}

		} else if (game_stage == LOST) {
			// death screen
			camera.CamHeight = 0.3f;
			camera.Pitch = 0.0f;
			lose_box.translate = camera.Position;
			lose_box.translate.x += 1.0f * camera.Front.x;
			lose_box.translate.y += camera.Front.y;
			lose_box.translate.z += 1.0f * camera.Front.z;
			lose_box.yAngle = -camera.Yaw;
			lose_box.xAngle = 90.0f;
			lose_box.render(lighting_shader, VAO_box);

		} else if (game_stage == WON) {
			// win screen
			win_box.translate = camera.Position;
			win_box.translate.x += 1.0f * camera.Front.x;
			win_box.translate.y += camera.Front.y;
			win_box.translate.z += 1.0f * camera.Front.z;
			win_box.yAngle = -camera.Yaw;
			win_box.xAngle = 90.0f;
			win_box.render(lighting_shader, VAO_box);
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
	if (door_stage > stage && door_anim[stage] < 1.0f) {
		door_anim[stage] = min(1.0f, door_anim[stage] + delta_time / 2);
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
	// Window closing
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (game_stage == PLAYING) {
		// Sprinting animation
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			sprint_anim = min(1.0f, sprint_anim + delta_time * 3);
		} else {
			sprint_anim = max(0.0f, sprint_anim - delta_time * 3);
		}
		camera.MovementSpeed = SPEED + SPEED * sin(glm::radians(sprint_anim * 90.0f));
		camera.Fov = 10.0f * sin(glm::radians(sprint_anim * 90.0f)) + fov;

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
			if (checkBoxRange(button_1_box, BTN_RANGE)) {
				//toggle button 1 (wood door)
				input_delay = INPUT_MAX;
				button_pressed[WOOD] = true;
				if (door_stage == WOOD) {
					door_stage = BRICK;
				}
			} else if (checkBoxRange(button_2_box_brick, BTN_RANGE)) {
				//toggle button 2 (brick door)
				input_delay = INPUT_MAX;
				button_pressed[BRICK] = true;
				if (door_stage == BRICK) {
					door_stage = MARBLE;
				}
			} else if (checkBoxRange(button_2_box_metal, BTN_RANGE)) {
				// false button2 (metal)
				input_delay = INPUT_MAX;
				button_2_metal_pressed = true;
			} else if (checkBoxRange(button_2_box_wood, BTN_RANGE)) {
				// false button2 (wood)
				input_delay = INPUT_MAX;
				button_2_wood_pressed = true;
			} else if (checkBoxRange(button_3_box_3, BTN_RANGE)) {
				// toggle button 3 (marble graffiti door)
				input_delay = INPUT_MAX;
				button_pressed[MARBLE] = true;
				if (door_stage == MARBLE) {
					door_stage = METAL;
				}
			} else if (checkBoxRange(button_3_box_2, BTN_RANGE)) {
				// false button3 (1+2=2)
				input_delay = INPUT_MAX;
				button_3_2_pressed = true;
			} else if (checkBoxRange(button_3_box_4, BTN_RANGE)) {
				// false button3 (1+2=4)
				input_delay = INPUT_MAX;
				button_3_4_pressed = true;
			} else if (
				checkBoxRange(button_4_box_1, BTN_RANGE)
				|| checkBoxRange(button_4_box_2, BTN_RANGE)
				|| checkBoxRange(button_4_box_3, BTN_RANGE)
			) {
				// toggle button 4's (all of them) (metal door)
				input_delay = INPUT_MAX;
				button_pressed[METAL] = true;
				if (door_stage == METAL) {
					door_stage = FINISH;
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
		perspective_proj = !perspective_proj;
	}

	// toggle extra brightness
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && input_delay == 0) {
		input_delay = INPUT_MAX;
		extra_bright = !extra_bright;
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
	camera.Position = glm::vec3(0.0f, CAM_HEIGHT, 3.0f);
	camera.CamHeight = CAM_HEIGHT;
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;
	firstMouse = true;
	fov = 60.0f;
	camera.Yaw = -90.0f;
	camera.Pitch = 0.0f;
	camera.updateCameraVectors();

	bad_guy.CamHeight = CAM_HEIGHT;
	bad_guy.Position = glm::vec3(0.0f, CAM_HEIGHT, 9.0f);

	sprint_anim = 0.0f;

	input_delay = 0;

	// Light
	light_radius = 1.2f;
	lamp_carrying = false;
	light_pos = LIGHT_INITIAL_POS;

	//Toggle (Animation or states)
	for (int i = 0; i < 4; i++) {
		button_pressed[i] = false;
	}
	button_2_metal_pressed = false;
	button_2_wood_pressed = false;
	button_3_2_pressed = false;
	button_3_4_pressed = false;

	perspective_proj = true;

	extra_bright = false;

	// Which door is next
	door_stage = WOOD;

	// Door animations
	for (int i = 0; i < 4; i++) {
		door_anim[i] = 0.0f;
	}

	// Colliders
	colliders[0] = &woodDoor;
	colliders[1] = &brick_door;
	colliders[2] = &marble_door;
	colliders[3] = &metal_door_bottom;

	game_stage = PLAYING;
}

void computeBounds(float& minX, float& maxX, float& minZ, float& maxZ, float pad, glm::vec3 pos) {
	// Start with bounds of static walls
	minX = left_wal.getPositiveBounds().x + pad;
	maxX = right_wall.getNegativeBounds().x - pad;
	minZ = floor_box.getNegativeBounds().z + pad;
	maxZ = floor_box.getPositiveBounds().z - pad;

	for (int i = 0; i < colliders_len; i++) {
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
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const* path) {

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
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (game_stage == PLAYING) {

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
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (game_stage == PLAYING) {
		camera.ProcessMouseScroll(yoffset);
		fov = camera.Fov;
	}
}
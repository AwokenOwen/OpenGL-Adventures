#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

#include "Shader.h"
#include "Camera.h"

//Function Protoypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Shader* shader);
unsigned int loadJPG(const char* path);
unsigned int loadPNG(const char* path);

//Variables
	// Screen
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
	//Camera Init
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f); 
	//Mouse Movement
float lastX = 400, lastY = 300;
bool firstMouse = true;
	//Time Vars
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main() 
{
	//NOTE: Init glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//NOTE: Create window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Owen Knizak OpenGL Adventure", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//NOTE: Init GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//NOTE: Set OpenGL Viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//NOTE: Change Viewport on screen change
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Generate vertices and store it in the buffer
	float vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)

	};

	unsigned int indices[] = { // note that we start from 0!
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};


	glEnable(GL_DEPTH_TEST);

#pragma region Vertex Array and Buffer SetUp
	//Generate the ID of a Vertex Array Object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	//Generating the ID of a Vertex Buffer Object
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	//Generating the ID of the Element Buffer Object for the indices
	unsigned int EBO;
	glGenBuffers(1, &EBO);

	//Bind the Vertex Array
	glBindVertexArray(VAO);

	//Bind the buffer to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Copy the data in GL_ARRAY_BUFFER into memory of the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Bind the buffer to the GL_ELEMENT_ARRAY_BUFFER target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//Copy the data in GL_ELEMENT_ARRAY_BUFFER into the memory of the GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		GL_STATIC_DRAW);

	//Set the position attribute for the shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)0);
	//enable the first posistion attribute
	glEnableVertexAttribArray(0);
	//Set the position attribute for the shader
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(3 * sizeof(float)));
	//enable the second posistion attribute
	glEnableVertexAttribArray(1);
	//Set the position attribute for the shader
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)(6 * sizeof(float)));
	//enable the third posistion attribute
	glEnableVertexAttribArray(2);
#pragma endregion

	unsigned int LightVAO;
	glGenVertexArrays(1, &LightVAO);

	glBindVertexArray(LightVAO);

	//Set the position attribute for the shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		(void*)0);
	//enable the first posistion attribute
	glEnableVertexAttribArray(0);	

	unsigned int diffuseMap = loadPNG("assets/container2.png");
	unsigned int specularMap = loadPNG("assets/container2_specular.png");

	Shader cubeShader("assets/program.vert", "assets/program.frag");
	Shader lightShader("assets/program.vert", "assets/lighting.frag");

	while (!glfwWindowShouldClose(window))
	{
		//Time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		cubeShader.use();

		//input
		processInput(window, &cubeShader);

		//rendering commands
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		//Directional Light Uniforms
		glm::vec3 DirectionalDiffuse = glm::vec3(0.75f, 0.75f, 0.25f);
		glm::vec3 DirectionalSpecular = glm::vec3(1.0f, 1.0f, 0.75f);

		glm::vec3 PointDiffuse = glm::vec3(0.5f, 0.5f, 0.38f);

		cubeShader.setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
		cubeShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.15f);
		cubeShader.setVec3("dirLight.diffuse", DirectionalDiffuse);
		cubeShader.setVec3("dirLight.specular", DirectionalSpecular);

		//Point Lights Uniforms
		cubeShader.setVec3("pointLights[0].position", pointLightPositions[0]); 
		cubeShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		cubeShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		cubeShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		 
		cubeShader.setFloat("pointLights[0].constant", 1.0f); 
		cubeShader.setFloat("pointLights[1].constant", 1.0f);
		cubeShader.setFloat("pointLights[2].constant", 1.0f);
		cubeShader.setFloat("pointLights[3].constant", 1.0f);

		cubeShader.setFloat("pointLights[0].linear", 0.35f); 
		cubeShader.setFloat("pointLights[1].linear", 0.35f);
		cubeShader.setFloat("pointLights[2].linear", 0.35f);
		cubeShader.setFloat("pointLights[3].linear", 0.35f);

		cubeShader.setFloat("pointLights[0].quadratic", 0.44f);
		cubeShader.setFloat("pointLights[1].quadratic", 0.44f);
		cubeShader.setFloat("pointLights[2].quadratic", 0.44f);
		cubeShader.setFloat("pointLights[3].quadratic", 0.44f);

		cubeShader.setVec3("pointLights[0].ambient", 0.2f, 0.2f, 0.15f);
		cubeShader.setVec3("pointLights[1].ambient", 0.2f, 0.2f, 0.15f);
		cubeShader.setVec3("pointLights[2].ambient", 0.2f, 0.2f, 0.15f);
		cubeShader.setVec3("pointLights[3].ambient", 0.2f, 0.2f, 0.15f);
		
		cubeShader.setVec3("pointLights[0].diffuse", PointDiffuse);
		cubeShader.setVec3("pointLights[1].diffuse", PointDiffuse);
		cubeShader.setVec3("pointLights[2].diffuse", PointDiffuse);
		cubeShader.setVec3("pointLights[3].diffuse", PointDiffuse);
		
		cubeShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 0.75f);
		cubeShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 0.75f);
		cubeShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 0.75f);
		cubeShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 0.75f);

		//View Uniform
		cubeShader.setVec3("viewPos", camera.Position);

		//Material Uniforms
		cubeShader.setInt("material.diffuse", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		cubeShader.setInt("material.specular", 1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		cubeShader.setFloat("material.shininess", 32.0f);


		//Get View Matrix
		glm::mat4 view = camera.GetViewMatrix(); 

		//Get Perspective Projection Matrix
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f,
			100.0f); 

		glBindVertexArray(VAO);
		for (int i = 0; i < 10; i++)
		{
			//Get World Transform Matrix
				//Get Identity Matrix
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(cubePositions[i]));
			float angle = i * 20.0f;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 1.2f));

			cubeShader.setMat4("model", model);
			cubeShader.setMat4("view", view);
			cubeShader.setMat4("projection", projection);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(LightVAO);
		lightShader.use();
		for (int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);

			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));

			lightShader.setMat4("model", model); 
			lightShader.setMat4("view", view); 
			lightShader.setMat4("projection", projection);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

//NOTE: Change Viewport on screen change
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed: y ranges bottom to top
	lastX = xpos;
	lastY = ypos;
	
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


void processInput(GLFWwindow* window, Shader* shader)
{
	//Quit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//Move Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime); 
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime); 
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime); 
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		shader->setVec3("flashlights[0].position", camera.Position);
		shader->setVec3("flashlights[0].direction", camera.Front);
		shader->setVec3("flashlights[0].diffuse", glm::vec3(1.0f));
		shader->setVec3("flashlights[0].specular", glm::vec3(1.0f));

		shader->setFloat("flashlights[0].cutOff", glm::cos(glm::radians(12.5f)));
		shader->setFloat("flashlights[0].outerCutOff", glm::cos(glm::radians(30.0f)));
		shader->setFloat("flashlights[0].power", 50.0f);
	}
	else
	{
		shader->setFloat("flashlights[0].power", 0.0f);
	}
}

unsigned int loadJPG(const char* path) 
{
	//Loading Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 
	// set the texture wrapping/filtering options (on currently bound texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, 
		&nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, 
			GL_UNSIGNED_BYTE, data); 
		glGenerateMipmap(GL_TEXTURE_2D); 
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl; 
	}
	stbi_image_free(data); 

	return texture;
}

unsigned int loadPNG(const char* path)
{
	//Loading Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on currently bound texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels; 
	unsigned char* data = stbi_load(path, &width, &height, 
		&nrChannels, 0); 
	if (data) 
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return texture;
}
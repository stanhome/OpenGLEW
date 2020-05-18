﻿#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/commonDefine.h"
#include "renderer/Texture.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"
#include "renderer/Model.h"

// settings
const int WIDTH = 800;
const int HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double s_lastX, s_lastY;

// timing
// time between current frame and last frame
float s_deltaTime = 0.0f;
float s_lastFrame = 0.0f;

//////////////////////////////////////////////////

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

/////////////////////////////////////////////////////////////////////////////////
void configOpenglGlobalState() {
	glEnable(GL_DEPTH_TEST);
}

GLFWwindow *createWindow(int w, int h) {
	Texture::initStbi();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// 告诉GLFW我们使用的是核心模式(Core-profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	// 如果使用的是Mac OS X系统，你还需要加下面这行代码到你的初始化代码中这些配置才能起作用：
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	GLFWwindow *window = glfwCreateWindow(w, h, "OpenGL Demo", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return nullptr;
	}

	glfwMakeContextCurrent(window);
	//glfwSetKeyCallback(window, keyCallback);

	// set mouse moving to handle camera rotation
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwGetCursorPos(window, &s_lastX, &s_lastY);
	// set mouse scrolling to handle camera FOV
	glfwSetScrollCallback(window, scrollCallback);

	// tel GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// init GLAD
	// GLFW gives us glfwGetProcAddress that defines the correct function based on which OS we're compiling for
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

		return nullptr;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);



	configOpenglGlobalState();

	return window;
}

//////////////////////////////////////
// event callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

// key event method 2.
//void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	/*
	- Calculate the mouse's offset since the last frame.
	- Add the offset values to the camera's yaw and pitch values.
	- Add some constraints to the minimum/maximum pitch values.
	- Calculate the direction vector.
	*/

	float xoffset = xpos - s_lastX, yoffset = ypos - s_lastY;
	s_lastX = xpos;
	s_lastY = ypos;

	camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}

///////////////////////////////////////
// call from main loop
typedef void OnProcessInput(GLFWwindow *window);
static OnProcessInput *s_processInputFunc = nullptr;

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, s_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, s_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, s_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, s_deltaTime);

	if (s_processInputFunc)
	{
		(*s_processInputFunc)(window);
	}
}



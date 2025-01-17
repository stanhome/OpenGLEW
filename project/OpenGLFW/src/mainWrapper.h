﻿#pragma once
#include <functional>
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

#include "common/log.h"
#include "renderer/fonts/ShowInfo.h"

// settings
const int WIDTH = 1920;
const int HEIGHT = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
double s_lastX, s_lastY;

// timing
// time between current frame and last frame
float s_deltaTime = 0.0f;
float s_lastFrame = 0.0f;

//////////////////////////////////////////////////

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

///////////////////////////////////////////////////
// renders a 1x1 XY quad in NDC
unsigned int s_quadVAO = 0;
unsigned int s_quadVBO = 0;
void renderScreenQuad() {
	if (s_quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// setup plane VAO
		glGenVertexArrays(1, &s_quadVAO);
		glBindVertexArray(s_quadVAO);
		glGenBuffers(1, &s_quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		unsigned int stride = 5 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}

	glBindVertexArray(s_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

/////////////////////////////////////////////////////////////////////////////////
void configOpenglGlobalState() {
	glEnable(GL_DEPTH_TEST);
}

GLFWwindow *createWindow(int w, int h) {
	Log::instance()->d("init begin, createWindow...");
	Texture::initStbi();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// 告诉GLFW我们使用的是核心模式(Core-profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glad_set_post_callback(glad_post_call_callback);

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

	show::ShowInfo::instance()->init(0, height - 10);
	show::ShowInfo::instance()->onScreenChangeSize(width, height);
	show::ShowInfo::instance()->setContent("FPS:60");

	Log::instance()->d("init done, createWindow end.");

	return window;
}

//////////////////////////////////////
// event callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);

	show::ShowInfo::instance()->onScreenChangeSize(width, height);
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

typedef void OnKeyClickEvent(int key);
static std::function<OnKeyClickEvent> s_onKeyClickEvent;
static std::map<int, bool> s_keyPressedMap;

bool s_isLPressed = false;

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

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) s_isLPressed = true;
	if (s_isLPressed && glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
	{
		s_isLPressed = false;
		camera.switchLock();
	}

	if (s_processInputFunc)
	{
		(*s_processInputFunc)(window);
	}

	if (s_onKeyClickEvent)
	{
		// handle key click event
		for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++)
		{
			if (glfwGetKey(window, key) == GLFW_PRESS) s_keyPressedMap[key] = true;
		}

		for (auto keyIter = s_keyPressedMap.begin(); keyIter != s_keyPressedMap.end(); ++keyIter)
		{
			if (keyIter->second == true && glfwGetKey(window, keyIter->first) == GLFW_RELEASE)
			{
				keyIter->second = false;
				(s_onKeyClickEvent)(keyIter->first);
			}
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// changed environment map file logic.
static const char *s_envFileArr[] = {
	"res/imgs/hdr/QueenMary_Chimney_Ref.hdr",
	"res/imgs/hdr/Circus_Backstage_3k.hdr",
	"res/imgs/hdr/Newport_Loft_Ref.hdr",
	"res/imgs/hdr/Factory_Catwalk_2k.hdr",
	"res/imgs/hdr/Theatre-Side_2k.hdr",
	"res/imgs/hdr/Mans_Outside_2k.hdr",
};
static int s_envFileIdx = 0;
static const char *s_currentEnvFile = s_envFileArr[0];

void changeEnvFile() {
	int envFileCount = sizeof(s_envFileArr) / sizeof(*s_envFileArr);
	s_envFileIdx++;
	s_envFileIdx %= envFileCount;

	s_currentEnvFile = s_envFileArr[s_envFileIdx];
}



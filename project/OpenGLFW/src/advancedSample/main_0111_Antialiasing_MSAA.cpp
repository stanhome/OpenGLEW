#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"

#include <stb_image.h>

using namespace std;

#define SHADER_PATH(SHADER_NAME) "res/shaders/03_advancedOpenGL/" SHADER_NAME

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	Shader cubeShader(SHADER_PATH("11_cube_default.vs"), SHADER_PATH("11_cube_default.fs"));

	// 3D obj
	CubeMesh cubeObj;

	glEnable(GL_MULTISAMPLE);

	// RENDER loop
	while (!glfwWindowShouldClose(window)) {
		// pre-frame time logic
		// ------------------------------
		// keep the same movement speed in different hardware PC.
		float currentFrame = glfwGetTime();
		s_deltaTime = currentFrame - s_lastFrame;
		s_lastFrame = currentFrame;

		//input
		// ------------------------------
		processInput(window);

		// Render
		// ------------------------------
		{
			// do Rendering
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 matrixV = camera.getViewMatrix();
			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);

			// draw planet
			cubeShader.use();
			glm::mat4 matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(0.0f, -3.0f, 0.0f));
			matrixM = glm::scale(matrixM, glm::vec3(4.0f));
			glm::mat4 matrixMVP = matrixP * matrixV * matrixM;
			cubeShader.setMat4("MVP", matrixMVP);
			cubeObj.draw(cubeShader);
		}

		// swap buffers and poll IO events(keys pressed/released, mouse moved etc.)
		// ------------------------------
		{
			// check and call events and swap the buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}


	// glfw: terminate, clearing all previously allocated GLFW resource.
	glfwTerminate();

	return 0;
}

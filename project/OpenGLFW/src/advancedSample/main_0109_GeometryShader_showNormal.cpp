#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"

#include <stb_image.h>

using namespace std;

static float s_points[] = {
	//screen pos,	color
	-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
	0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
	-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
};

#define SHADER_PATH(SHADER_NAME) "res/shaders/03_advancedOpenGL/" SHADER_NAME

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	// 3D obj
	Shader objShader(SHADER_PATH("09_model_default.vs"), SHADER_PATH("09_model_default.fs"));
	Shader normalShader(SHADER_PATH("09_geometry_normal_visualization.vs")
		, SHADER_PATH("09_geometry_normal_visualization.fs")
		, SHADER_PATH("09_geometry_normal_visualization.gs"));

	Model backpack("res/objects/backpack/backpack.obj");

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

			// config transformation matrices
			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
			glm::mat4 matrixV = camera.getViewMatrix();
			glm::mat4 matrixM = glm::mat4(1.0f);

			objShader.use();
			glm::mat4 objMatrixMVP = matrixP * matrixV * matrixM;
			objShader.setMat4("MVP", objMatrixMVP);
			// draw model as usual
			backpack.draw(objShader);

			// draw model with normal visualizing geometry shader.
			normalShader.use();
			normalShader.setMat4("MVP", objMatrixMVP);
			glm::mat4 objMatrixMV = matrixV * matrixM;
			normalShader.setMat4("MV", objMatrixMV);
			normalShader.setMat4("P", matrixP);
			backpack.draw(normalShader);
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

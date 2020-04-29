#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"

using namespace std;

static float s_cubeVertices[] = {
	// Back face
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	// Front face
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	// Left face
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	// Right face
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
	// Bottom face
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	// Top face
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left 
};

#define SHADER_PATH(SHADER_NAME) "res/shaders/03_advancedOpenGL/" SHADER_NAME

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);// by default
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BACK);
	glFrontFace(GL_CCW);


	// 3D obj
	Shader objShader(SHADER_PATH("01_depthTesting.vs"), SHADER_PATH("03_blending_blend.fs"));

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// cube VAO
	unsigned int cubeVao, cubeVbo;
	glGenVertexArrays(1, &cubeVao);
	glGenBuffers(1, &cubeVbo);
	glBindVertexArray(cubeVao);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_cubeVertices), &s_cubeVertices, GL_STATIC_DRAW);
	unsigned int stride = 5 * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	Texture cubeTexture(GL_TEXTURE_2D, "res/imgs/marble.jpg");

	objShader.use();
	objShader.setInt("_mainTex", 0);

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

			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
			glm::mat4 matrixV = camera.getViewMatrix();
			glm::mat4 matrixM = glm::mat4(1.0f);

			objShader.use();

			// draw cubes
			glBindVertexArray(cubeVao);
			cubeTexture.bind(GL_TEXTURE0);
			matrixM = glm::translate(matrixM, glm::vec3(-1.0f, 0.0f, -1.0f));
			glm::mat4 objMatrixMVP = matrixP * matrixV * matrixM;
			objShader.setMat4("MVP", objMatrixMVP);
			glDrawArrays(GL_TRIANGLES, 0, 36);

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


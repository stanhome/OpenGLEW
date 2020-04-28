#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"

using namespace std;

static float s_cubeVertices[] = {
	// positions          // texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

static float s_planeVertices[] = {
	// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

	5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

static float s_transparentVertices[] = {
	// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
	0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.0f, -0.5f,  0.0f,  0.0f,  0.0f,
	1.0f, -0.5f,  0.0f,  1.0f,  0.0f,

	0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
	1.0f, -0.5f,  0.0f,  1.0f,  0.0f,
	1.0f,  0.5f,  0.0f,  1.0f,  1.0f
};

#define SHADER_PATH(SHADER_NAME) "res/shaders/03_advancedOpenGL/" SHADER_NAME

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);// by default


	// 3D obj
	Shader objShader(SHADER_PATH("01_depthTesting.vs"), SHADER_PATH("03_blending_discard.fs"));

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

	// plane VAO
	unsigned int planeVao, PlaneVbo;
	glGenVertexArrays(1, &planeVao);
	glGenBuffers(1, &PlaneVbo);
	glBindVertexArray(planeVao);
	glBindBuffer(GL_ARRAY_BUFFER, PlaneVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_planeVertices), &s_planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// transparent VAO
	unsigned int transparentVao, transparentVbo;
	glGenVertexArrays(1, &transparentVao);
	glGenBuffers(1, &transparentVbo);
	glBindVertexArray(transparentVao);

	glBindBuffer(GL_ARRAY_BUFFER, transparentVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_transparentVertices), &s_transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 *sizeof(float)));

	glBindVertexArray(0);

	// load textures
	Texture cubeTexture(GL_TEXTURE_2D, "res/imgs/marble.jpg");
	Texture floorTexture(GL_TEXTURE_2D, "res/imgs/metal.jpg", GL_REPEAT);
	Texture transparentTexture(GL_TEXTURE_2D, "res/imgs/grass.png", GL_CLAMP_TO_EDGE);

	//transparent vegetation locations
	vector<glm::vec3> vegetation{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

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

			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(2.0f, 0.0f, 0.0f));
			objMatrixMVP = matrixP * matrixV * matrixM;
			objShader.setMat4("MVP", objMatrixMVP);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// floor
			glBindVertexArray(planeVao);
			floorTexture.bind(GL_TEXTURE0);
			matrixM = glm::mat4(1.0f);
			objMatrixMVP = matrixP * matrixV * matrixM;
			objShader.setMat4("MVP", objMatrixMVP);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// vegetation
			glBindVertexArray(transparentVao);
			transparentTexture.bind(GL_TEXTURE0);
			for (unsigned int i = 0; i < vegetation.size(); ++i)
			{
				matrixM = glm::mat4(1.0f);
				matrixM = glm::translate(matrixM, vegetation[i]);
				objMatrixMVP = matrixP * matrixV * matrixM;
				objShader.setMat4("MVP", objMatrixMVP);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

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


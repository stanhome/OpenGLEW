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

	// setup quad for screen VAO
	unsigned int quadVao, quadVbo;
	glGenVertexArrays(1, &quadVao);
	glBindVertexArray(quadVao);

	glGenBuffers(1, &quadVbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_quadVertices), &s_quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

	glBindVertexArray(0);

	// config MSAA framebuffer
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create a multisampled color attachment texture
	unsigned int textureColorBufferMultisampled;
	glGenTextures(1, &textureColorBufferMultisampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultisampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, WIDTH, HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultisampled, 0);
	// create a (also mutisampled) renderbuffer object for depth and stencil attachments
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "[E] framebuffer is not complete for multisampled" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

			// 1. draw scene as normal in multisampled buffers;
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glEnable(GL_DEPTH_TEST);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// set transform matrices
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

		{
			// render screen quad
			// 2. now blit multisampled buffers to normal color buffer of intermediate 
			

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

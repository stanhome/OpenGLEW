#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"
#include "renderer/PlaneMesh.h"

#include <stb_image.h>

using namespace std;

#define SHADER_PATH(SHADER_NAME) "res/shaders/04_advancedLighting/" SHADER_NAME

//static bool s_isBlinn = false;
//static bool s_isBlinnKeyPressed = false;

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	Shader simpleDepthShader(SHADER_PATH("03_shadowMappingDepth.vs"), SHADER_PATH("03_shadowMappingDepth.fs"));
	Shader debugDepthQuad(SHADER_PATH("03_debugQuad.vs"), SHADER_PATH("03_debugQuad.fs"));

	// set up vertex data(and buffers) and configure vertex attributes
	// 3D obj
	Texture woodTex(GL_TEXTURE_2D, "res/imgs/wood.png");
	woodTex.setSamplerName("tex_diffuse", 0);
	PlaneMesh plane(std::make_shared<Texture>(woodTex));
	CubeMesh cube;

	// configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFbo;
	glGenFramebuffers(1, &depthMapFbo);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	// we explicitly tell OpenGL we're not going to render any color data.
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 );
	
	// shader confriguration
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);

	// lighting info
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

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
		//s_processInputFunc = [](GLFWwindow * w) -> void {
		//	if (glfwGetKey(w, GLFW_KEY_B) == GLFW_PRESS)
		//	{
		//		s_isBlinnKeyPressed = true;
		//	}

		//	if (s_isBlinnKeyPressed && glfwGetKey(w, GLFW_KEY_B) == GLFW_RELEASE) {
		//		s_isBlinnKeyPressed = false;
		//		s_isBlinn = ! s_isBlinn;
		//		cout << (s_isBlinn ? "Blinn-Phong" : "Phong") << endl;
		//	}
		//};

		// Render
		// ------------------------------
		// do Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspecitve)
		glm::mat4 matrixLightP, matrixLightV;
		glm::mat4 matrixLightSpace;
		float nearPlane = 1.0f, farPlane = 7.5f;
		matrixLightP = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
		matrixLightV = glm::lookAt(lightPos, V::zero, V::up);
		matrixLightSpace = matrixLightP * matrixLightV;
		// render scene from light's point of view
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", matrixLightSpace);

		//这里要调用一下 glViewport，因为 shadow maps 通常会有不同的分辨率
		glViewport(0, 0, SHADOW_HEIGHT, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			
			// render scene
			// floor
			glm::mat4 matrixM = glm::mat4(1.0f);
			matrixM = glm::scale(matrixM, glm::vec3(25, 0, 25));
			simpleDepthShader.setMat4("M", matrixM);
			plane.draw(simpleDepthShader);

			// cubes
			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(0.0f, 1.5f, 0.0f));
			matrixM = glm::scale(matrixM, glm::vec3(0.5f));
			simpleDepthShader.setMat4("M", matrixM);
			cube.draw(simpleDepthShader);

			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(2.0f, 0.0f, 1.0f));
			matrixM = glm::scale(matrixM, glm::vec3(0.5f));
			simpleDepthShader.setMat4("M", matrixM);
			cube.draw(simpleDepthShader);

			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(-1.0f, 0.0f, 2.0f));
			matrixM = glm::rotate(matrixM, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			matrixM = glm::scale(matrixM, glm::vec3(0.25f));
			simpleDepthShader.setMat4("M", matrixM);
			cube.draw(simpleDepthShader);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render depth map to quad for visual debuging
		debugDepthQuad.use();
		debugDepthQuad.setFloat("nearPlane", nearPlane);
		debugDepthQuad.setFloat("farPlane", farPlane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScreenQuad();

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


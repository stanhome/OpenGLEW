#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"
#include "renderer/PlaneMesh.h"

#include <stb_image.h>

using namespace std;

#define SHADER_PATH(SHADER_NAME) "res/shaders/04_advancedLighting/" SHADER_NAME

static bool s_isBlinn = false;
static bool s_isBlinnKeyPressed = false;

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	glEnable(GL_CULL_FACE);

	Shader simpleDepthShader(SHADER_PATH("03_01_pointShadowsDepth.vs"), SHADER_PATH("03_01_pointShadowsDepth.fs"), SHADER_PATH("03_01_pointShadowsDepth.gs"));
	Shader objShader(SHADER_PATH("03_01_pointShadowLighting.vs"), SHADER_PATH("03_01_pointShadowLighting.fs"));

	// set up vertex data(and buffers) and configure vertex attributes
	// 3D obj
	Texture woodTex(GL_TEXTURE_2D, "res/imgs/wood.png");
	woodTex.setSamplerName("tex_diffuse", 0);
	auto pWoodTex = std::make_shared<Texture>(woodTex);

	CubeMesh cube(pWoodTex);

	// configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFbo;
	glGenFramebuffers(1, &depthMapFbo);
	// create depth cubemap texture
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	// we explicitly tell OpenGL we're not going to render any color data.
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 );
	
	// shader configuration
	objShader.use();
	objShader.setInt("depthMap", 1);


	// lighting info
	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

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
		s_processInputFunc = [](GLFWwindow * w) -> void {
			if (glfwGetKey(w, GLFW_KEY_B) == GLFW_PRESS)
			{
				s_isBlinnKeyPressed = true;
			}

			if (s_isBlinnKeyPressed && glfwGetKey(w, GLFW_KEY_B) == GLFW_RELEASE) {
				s_isBlinnKeyPressed = false;
				s_isBlinn = !s_isBlinn;
				cout << (s_isBlinn ? "Blinn-Phong" : "Phong") << endl;
			}
		};

		// move light position over time


		// Render
		// ------------------------------
		// do Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 0. create depth cubemap transformation matrices
		float near_plane = 1.0f;
		float far_plane = 25.0f;
		glm::mat4 matrixShadowP = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		std::vector<glm::mat4> matrixShadowVPs;
		matrixShadowVPs.push_back(matrixShadowP * glm::lookAt(lightPos, lightPos + V::left, glm::vec3(0.0f, -1.0f, 0.0f)));
		matrixShadowVPs.push_back(matrixShadowP * glm::lookAt(lightPos, lightPos - V::left, glm::vec3(0.0f, -1.0f, 0.0f)));
		matrixShadowVPs.push_back(matrixShadowP * glm::lookAt(lightPos, lightPos + V::up, glm::vec3(0.0f, 0.0f, 1.0f)));
		matrixShadowVPs.push_back(matrixShadowP * glm::lookAt(lightPos, lightPos - V::up, glm::vec3(0.0f, 0.0f, -1.0f)));
		matrixShadowVPs.push_back(matrixShadowP * glm::lookAt(lightPos, lightPos + V::forward, glm::vec3(0.0f, -1.0f, 0.0f)));
		matrixShadowVPs.push_back(matrixShadowP * glm::lookAt(lightPos, lightPos - V::forward, glm::vec3(0.0f, -1.0f, 0.0f)));

		auto renderScene = [&cube](const Shader &shader) -> void {
			// render scene
			// room cube
			glm::mat4 matrixM = glm::mat4(1.0f);
			matrixM = glm::scale(matrixM, glm::vec3(5.0f));
			shader.setMat4("M", matrixM);
			// note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling
			glDisable(GL_CULL_FACE);
			// a small little hack to invert normals when drawing cube from the inside so lighting still works.
			shader.setFloat("isReverseNormal", 1);
			cube.draw(shader);

			// and of course disable it
			shader.setFloat("isReverseNormal", 0);
			glEnable(GL_CULL_FACE);

			// cubes
			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(4.0f, -3.5f, 0.0));
			matrixM = glm::scale(matrixM, glm::vec3(0.5f));
			shader.setMat4("M", matrixM);
			cube.draw(shader);

			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(2.0f, 3.0f, 1.0f));
			matrixM = glm::scale(matrixM, glm::vec3(0.75f));
			shader.setMat4("M", matrixM);
			cube.draw(shader);

			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(-3.0f, -1.0f, 0.0));
			matrixM = glm::scale(matrixM, glm::vec3(0.5f));
			shader.setMat4("M", matrixM);
			cube.draw(shader);

			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(-1.5f, 1.0f, 1.5));
			matrixM = glm::scale(matrixM, glm::vec3(0.5f));
			shader.setMat4("M", matrixM);
			cube.draw(shader);

			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(-1.5f, 2.0f, -3.0));
			matrixM = glm::rotate(matrixM, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			matrixM = glm::scale(matrixM, glm::vec3(0.75f));
			shader.setMat4("M", matrixM);
			cube.draw(shader);
		};

		// 1. render scene to depth cubemap
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			simpleDepthShader.use();
			for (unsigned int i = 0; i < 6; ++i)
				simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", matrixShadowVPs[i]);

			simpleDepthShader.setFloat("far_plane", far_plane);
			simpleDepthShader.setVec3("lightPos", lightPos);

			renderScene(simpleDepthShader);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. render scene as normal using the generated depth cubemap
		// reset viewport
		{
			glViewport(0, 0, WIDTH, HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			objShader.use();
			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
			glm::mat4 matrixV = camera.getViewMatrix();
			glm::mat4 matrixVP = matrixP * matrixV;
			objShader.setMat4("VP", matrixVP);

			// set light uniforms
			objShader.setVec3("viewPos", camera.pos);
			objShader.setVec3("lightPos", lightPos);
			objShader.setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
			objShader.setFloat("isBlinn", s_isBlinn);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			renderScene(objShader);
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


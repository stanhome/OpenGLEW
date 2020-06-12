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

void renderQuad();


int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	Shader shaderGeometryPass(SHADER_PATH("08_G-buffer.vs"), SHADER_PATH("08_G-buffer.fs"));
	Shader shaderLightingPass(SHADER_PATH("08_DeferredShading.vs"), SHADER_PATH("08_DeferredShading.fs"));
	Shader shaderLightBox(SHADER_PATH("08_lightBox.vs"), SHADER_PATH("08_lightBox.fs"));

	// 3D obj
	Model backpack("res/objects/backpack/backpack.obj");
	CubeMesh cube;

	std::vector<glm::vec3> objectPositions = std::vector<glm::vec3> {
		glm::vec3(-3.0, -0.5, -3.0),
		glm::vec3(0.0,  -0.5, -3.0),
		glm::vec3(3.0,  -0.5, -3.0),
		glm::vec3(-3.0,  -0.5,  0.0),
		glm::vec3(0.0,  -0.5,  0.0),
		glm::vec3(3.0,  -0.5,  0.0),
		glm::vec3(-3.0,  -0.5,  3.0),
		glm::vec3(0.0,  -0.5,  3.0),
		glm::vec3(3.0,  -0.5,  3.0),
	};

	// config G-buffer framebuffer
	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedoSpec;

	// position buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// albedo & specular buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// tell OpenGL which color attachments we'll use(of this framebuffer) for rendering
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// finally check if ramebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//lighting info
	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPosVec;
	std::vector<glm::vec3> lightColorVec;
	
	srand(glfwGetTime());
	for (unsigned int i = 0; i < NR_LIGHTS; ++i)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 4.0 - 2.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPosVec.push_back(glm::vec3(xPos, yPos, zPos));

		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5;
		float bColor = ((rand() % 100) / 200.0f) + 0.5;
		lightColorVec.push_back(glm::vec3(rColor, gColor, bColor));
	}

	shaderLightingPass.use();
	shaderLightingPass.setInt("gPosition", 0);
	shaderLightingPass.setInt("gNormal", 1);
	shaderLightingPass.setInt("gAlbedoSpec", 2);

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
				s_isBlinn = ! s_isBlinn;
				cout << (s_isBlinn ? "Blinn-Phong" : "Phong") << endl;
			}
		};

		// Render
		// ------------------------------
		// do Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. geometry pass: render scene's geometry/color data into G-buffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 matrixV = camera.getViewMatrix();
			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);
			glm::mat4 matrixM = glm::mat4(1.0f);
			glm::mat4 matrixVP = matrixP * matrixV;

			shaderGeometryPass.use();
			shaderGeometryPass.setMat4("VP", matrixVP);

			for (unsigned int i = 0; i < objectPositions.size(); ++i)
			{
				matrixM = glm::mat4(1.0f);
				matrixM = glm::translate(matrixM, objectPositions[i]);
				matrixM = glm::scale(matrixM, glm::vec3(0.5f));
				shaderGeometryPass.setMat4("M", matrixM);
				backpack.draw(shaderGeometryPass);
			}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the G-buffer's content.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderLightingPass.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

		// send light relevant uniforms
		for (unsigned int i = 0; i < lightPosVec.size(); ++i)
		{
			std::string lightsNamePrefixInShader = "lights[" + std::to_string(i) + "].";
			shaderLightingPass.setVec3(lightsNamePrefixInShader + "pos", lightPosVec[i]);
			shaderLightingPass.setVec3(lightsNamePrefixInShader + "color", lightColorVec[i]);
			// update attenuation parameters and calculate radius
			const float linear = 0.7;
			const float quadratic = 1.8;
			shaderLightingPass.setFloat(lightsNamePrefixInShader + "linear", linear);
			shaderLightingPass.setFloat(lightsNamePrefixInShader + "quadratic", quadratic);
		}
		shaderLightingPass.setVec3("viewPos", camera.pos);
		renderQuad();

		// 2.5 copy content of geometry's depth buffer to default framebuffer's  depth buffer.
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		// blit to default framebuffer.
		// Note that this may or may not works as internal formats of both the FBO and default framebuffer have to match.the internal formats are implementation defined.
		// This works on all of my systems, but if it doesn't on yours you'll likely to write to the depth buffer in another shader stage(or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. render lights on top of Scene for Debug
		shaderLightBox.use();
		glm::mat4 matrixMVP = glm::mat4(1.0f);
		for (unsigned int i = 0; i < lightPosVec.size(); ++i)
		{
			matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, lightPosVec[i]);
			matrixM = glm::scale(matrixM, glm::vec3(0.125f));
			matrixMVP = matrixVP * matrixM;
			shaderLightBox.setMat4("MVP", matrixMVP);
			shaderLightBox.setVec3("lightColor", lightColorVec[i]);
			cube.draw(shaderLightBox);
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

// renders a 1x1 XY quad in NDC
unsigned int s_quadVAO = 0;
unsigned int s_quadVBO = 0;
void renderQuad() {
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
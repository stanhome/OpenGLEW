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

	Shader planeShader(SHADER_PATH("01_Bliin-Phong.vs"), SHADER_PATH("01_Bliin-Phong.fs"));

	// 3D obj
	shared_ptr<Texture> planeTex = std::make_shared<Texture>(GL_TEXTURE_2D, "res/imgs/wood.png");
	planeTex->setSamplerName("tex_diffuse", 0);
	PlaneMesh planeObj(planeTex);

	//lighting info
	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

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
		{
			// do Rendering
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// set transform matrices
			glm::mat4 matrixV = camera.getViewMatrix();
			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);

			planeShader.use();
			glm::mat4 matrixM = glm::mat4(1.0f);
			matrixM = glm::scale(matrixM, glm::vec3(10, 1, 10));
			glm::mat4 matrixMVP = matrixP * matrixV * matrixM;
			planeShader.setMat4("MVP", matrixMVP);

			//set light uniforms
			planeShader.setVec3("viewPos", camera.pos);
			planeShader.setVec3("lightPos", lightPos);
			planeShader.setVec3("lightColor", lightColor);
			planeShader.setInt("isBlinn", s_isBlinn);

			planeObj.draw(planeShader);
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

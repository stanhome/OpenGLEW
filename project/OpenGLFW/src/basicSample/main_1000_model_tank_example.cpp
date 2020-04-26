#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"

using namespace std;

glm::vec3 s_pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3 s_pointLightColor[] = {
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
};

const unsigned int s_pointLightNumber = sizeof(s_pointLightPositions) / sizeof(*s_pointLightPositions);

#define SHADER_PATH(SHADER_NAME) "res/shaders/02_lighting/" SHADER_NAME

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	// 3D obj
	Shader objShader(SHADER_PATH("08_model_no_normalMap.vs"), SHADER_PATH("08_model_no_normalMap.fs"));
	Model myModel("res/objects/phoenix/phoenix_ugv.md2");
	//Model myModel("res/objects/nanosuit/nanosuit.obj");
	float modelScale = 0.02f;

	// lamp light
	Shader lampShader(SHADER_PATH("01_lamp.vs"), SHADER_PATH("01_lamp.fs"));
	CubeMesh lampObj = CubeMesh();

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
			glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
			glm::mat4 matrixV = camera.getViewMatrix();

			// render the obj
			objShader.use();
			objShader.setVec3("viewPos", camera.pos);

			objShader.setFloat("mat_shininess", 32.0f);

			//light properties
			// directional light
			objShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			objShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			objShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			objShader.setVec3("dirLight.specular", 0.5f);

			string keyPrefix = "pointLight.";
			objShader.setVec3(keyPrefix + "pos", s_pointLightPositions[0]);
			objShader.setVec3(keyPrefix + "ambient", 0.1f);
			objShader.setVec3(keyPrefix + "diffuse", s_pointLightColor[0]);
			objShader.setVec3(keyPrefix + "specular", 1.0f);
			objShader.setFloat(keyPrefix + "constant", 1.0f);
			objShader.setFloat(keyPrefix + "linear", 0.014f); // Distance:325，可以照325单位远的
			objShader.setFloat(keyPrefix + "quadratic", 0.0007f);

			glm::mat4 objMatrixM = glm::mat4(1.0f);
			// translate it down so it's at the center of the scene
			objMatrixM = glm::translate(objMatrixM, glm::vec3(0.0f, -1.75f, 0.0f));
			// it's a bit too big for our scene, so scale it down.
			objMatrixM = glm::scale(objMatrixM, glm::vec3(modelScale, modelScale, modelScale));
			glm::mat4 objMatrixMVP = matrixP * matrixV * objMatrixM;
			objShader.setMat4("MVP", objMatrixMVP);
			objShader.setMat4("M", objMatrixM);

			myModel.draw(objShader);

			// draw the lamp
			lampShader.use();
			glm::mat4 lampMatrixM = glm::translate(M::i, s_pointLightPositions[0]);
			lampMatrixM = glm::scale(lampMatrixM, glm::vec3(0.2f));
			glm::mat4 lampMatrixMVP = matrixP * matrixV * lampMatrixM;
			lampShader.setMat4("MVP", lampMatrixMVP);
			lampShader.setVec3("color", s_pointLightColor[0]);
			lampObj.draw(lampShader);
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


#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"
#include "renderer/PlaneMesh.h"
#include "renderer/SphereMesh.h"

#include <stb_image.h>

using namespace std;

#define SHADER_PATH(SHADER_NAME) "res/shaders/05_PBR/" SHADER_NAME

static bool s_isBlinn = false;
static bool s_isBlinnKeyPressed = false;

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	SphereMesh sphere;

	Texture woodTex(GL_TEXTURE_2D, "res/imgs/wood.png");
	woodTex.setSamplerName("tex_diffuse", 0);
	auto pWoodTex = std::make_shared<Texture>(woodTex);
	CubeMesh cube(pWoodTex);
	//Model backpack("res/objects/backpack/backpack.obj");

	Shader pbrShader(SHADER_PATH("01_PBR_lighting.vs"), SHADER_PATH("01_PBR_lighting.fs"));
	//Shader pbrShader(SHADER_PATH("1.1.pbr.vs"), SHADER_PATH("1.1.pbr.fs"));
	Shader lampShader("res/shaders/02_lighting/01_lamp.vs", "res/shaders/02_lighting/01_lamp.fs");

	pbrShader.use();
	pbrShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
	pbrShader.setFloat("ao", 1.0f);

	// lights
	glm::vec3 lightPosArr[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	// HDR
	glm::vec3 lightColorArr[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	int lightCount = sizeof(lightPosArr) / sizeof(lightPosArr[0]);

	int sphereRow = 7, sphereColumn = 7;
	float spacing = 2.5f;

	glm::mat4 matProjection = glm::perspective(glm::radians(camera.fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

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
			//if (glfwGetKey(w, GLFW_KEY_B) == GLFW_PRESS)
			//{
			//	s_isBlinnKeyPressed = true;
			//}

			//if (s_isBlinnKeyPressed && glfwGetKey(w, GLFW_KEY_B) == GLFW_RELEASE) {
			//	s_isBlinnKeyPressed = false;
			//	s_isBlinn = !s_isBlinn;
			//	cout << (s_isBlinn ? "Blinn-Phong" : "Phong") << endl;
			//}
		};

		// Render
		// ------------------------------
		// do Rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// pass the light properties
		for (unsigned int i = 0; i < lightCount; ++i)
		{
			glm::vec3 lightPos = lightPosArr[i];
			std::string idxStr = std::to_string(i);
			pbrShader.use();
			pbrShader.setVec3("lightPosArr[" + idxStr + "]", lightPos);
			pbrShader.setVec3("lightColorArr[" + idxStr + "]", lightColorArr[i]);

			// debug light
			lampShader.use();
			glm::mat4 matM = glm::mat4(1.0f);
			matM = glm::translate(matM, lightPos);
			matM = glm::scale(matM, glm::vec3(0.5f));

			glm::mat4 matMVP = matProjection * camera.getViewMatrix() * matM;
			lampShader.setMat4("MVP", matMVP);
			lampShader.setVec3("color", lightColorArr[i]);
			sphere.draw(lampShader);
		}

		pbrShader.use();
		glm::mat4 matVP = matProjection * camera.getViewMatrix();
		pbrShader.setMat4("VP", matVP);
		pbrShader.setVec3("viewPos", camera.pos);

		// render row * column number of spheres with varying metallic/roughness values scaled by rows and columns respectively.
		glm::mat4 M = glm::mat4(1.0f);
		for (int row = 0; row < sphereRow; ++row)
		{
			pbrShader.setFloat("metallic", (float)row / (float)sphereRow);
			for (int col = 0; col < sphereColumn; ++col)
			{
				//we clamp the roughness to 0.025 - 1.0 as perfectly smooth surface(roughness of 0.0) tend to look a bit off on direct lighting.
				pbrShader.setFloat("roughness", glm::clamp((float)col / (float)sphereColumn, 0.05f, 1.0f));

				M = glm::mat4(1.0f);
				M = glm::translate(M, glm::vec3(
					(col - (sphereColumn / 2)) * spacing,
					(row - (sphereRow / 2)) * spacing,
					0.0f
				));

				pbrShader.setMat4("M", M);
				// render sphere
				sphere.draw(pbrShader);
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


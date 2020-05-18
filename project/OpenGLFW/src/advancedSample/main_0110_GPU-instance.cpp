#include <iostream>
#include <sstream>

#include "mainWrapper.h"
#include "renderer/CubeMesh.h"

#include <stb_image.h>

using namespace std;

#define SHADER_PATH(SHADER_NAME) "res/shaders/03_advancedOpenGL/" SHADER_NAME

// return [-offset, offset), offset should be positive number.
float generateRandomScope(float offset) {
	return rand() % (int)(2 * offset * 100) / 100.0f - offset;
}

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	// 3D obj
	Shader planetShader(SHADER_PATH("10_planet-with-common.vs"), SHADER_PATH("10_planet-with-common.fs"));
	Shader rockShader(SHADER_PATH("10_planet_with-GPU-instance.vs"), SHADER_PATH("10_planet-with-common.fs"));
	Model rock("res/objects/rock/rock.obj");
	Model planet("res/objects/planet/planet.obj");

	// generate a large list of semi-random model transformation matrices
	unsigned int amount = 1000;
	glm::mat4 *matrixMArray;
	matrixMArray = new glm::mat4[amount];
	srand(glfwGetTime());
	float radius = 50.0;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; ++i)
	{
		glm::mat4 M = glm::mat4(1.0f);

		// 1. translation displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float x = sin(angle) * radius + generateRandomScope(offset);
		// keep height of asteroid field smaller compared to width of x and z
		float y = 0.4f * generateRandomScope(offset);
		float z = cos(angle) * radius + generateRandomScope(offset);
		M = glm::translate(M, glm::vec3(x, y, z));

		// 2. scale between 0.05 and 0.25
		float scale = (rand() % 20) / 100.0f + 0.05;
		M = glm::scale(M, glm::vec3(scale));

		// 3 rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = rand() % 360;
		M = glm::rotate(M, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		matrixMArray[i] = M;
	}

	// set transformation matrices as an instance vertex attribute( with divisor 1)
	// 顶点属性最大的数据 >= 一个vec4，所以 mat4 要处理为4个顶点属性。 
	// 当mat4 设置为3的时候，矩阵每一列的顶点属性位置就是3、4、5、6
	unsigned int vertexInstanceBuffer;
	glGenBuffers(1, &vertexInstanceBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexInstanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &matrixMArray[0], GL_STATIC_DRAW);
	for (unsigned int i = 0; i < rock.meshes.size(); ++i)
	{
		unsigned int vao = rock.meshes[i]->vao;
		glBindVertexArray(vao);
		// set attribute pointers for matrix(4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 *sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

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

			glm::mat4 matrixV = camera.getViewMatrix();
			glm::mat4 matrixP = glm::perspective(glm::radians(camera.fov), (float)WIDTH / HEIGHT, 0.1f, 1000.0f);

			// draw planet
			planetShader.use();
			glm::mat4 matrixM = glm::mat4(1.0f);
			matrixM = glm::translate(matrixM, glm::vec3(0.0f, -3.0f, 0.0f));
			matrixM = glm::scale(matrixM, glm::vec3(4.0f));
			glm::mat4 matrixMVP = matrixP * matrixV * matrixM;
			planetShader.setMat4("MVP", matrixMVP);
			planet.draw(planetShader);

			// draw meteorites
			rockShader.use();
			rockShader.setInt("tex_diffuse0", 0);
			glm::mat4 matrixVP = matrixP * matrixV;
			rockShader.setMat4("VP", matrixVP);
			for (unsigned int i = 0; i < rock.meshes.size(); ++i)
			{
				std::shared_ptr<Mesh> mesh = rock.meshes[i];

				mesh->textures[0]->bind(GL_TEXTURE0);
				glBindVertexArray(mesh->vao);
				glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, amount);
				glBindVertexArray(0);
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

	delete[] matrixMArray;

	// glfw: terminate, clearing all previously allocated GLFW resource.
	glfwTerminate();

	return 0;
}

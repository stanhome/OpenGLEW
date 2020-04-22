#include <iostream>
#include <sstream>

#include "mainWrapper.h"

using namespace std;

static const GLfloat s_vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

glm::vec3 s_cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 s_pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3 s_pointLightColor[] = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
};

const unsigned int s_pointLightNumber = sizeof(s_pointLightPositions) / sizeof(*s_pointLightPositions);

#define SHADER_PATH(SHADER_NAME) "res/shaders/02_lighting/" SHADER_NAME

int main()
{
	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	Shader objShader(SHADER_PATH("05_lightCasters.vs"), SHADER_PATH("06_multiple_lights.fs"));
	Shader lampShader(SHADER_PATH("01_lamp.vs"), SHADER_PATH("01_lamp.fs"));

	// generate render object
	// 1.configure the cube's VAO(and VBO)
	GLuint vbo, vaoCube;
	glGenVertexArrays(1, &vaoCube);
	glGenBuffers(1, &vbo);

	// copy our vertices array in a buffer for OpenGL to use.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

	Texture diffuseMap(GL_TEXTURE_2D, "res/imgs/container2.png", GL_RGBA);
	Texture specularMap(GL_TEXTURE_2D, "res/imgs/container2_specular.png", GL_RGBA);

	glBindVertexArray(vaoCube);
	int stride = 8 * sizeof(GLfloat);
	// pos attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Unbind VAO  Unbind VAO(it's always a good thing to unbind any buffer/array to prevent strange bugs) 
	glBindVertexArray(0);

	// 2. configure the lights' VAO( VBO stays the same; the vertices are the same for the light which is also a 3D cube)
	GLuint vaoLight;
	glGenVertexArrays(1, &vaoLight);
	glBindVertexArray(vaoLight);
	//we only need to bind to the VBO(to link it with glVertexAttribPointer), no need to fill it.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

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

			// render the cube
			objShader.use();
			objShader.setVec3("viewPos", camera.pos);


			// material properties
			// now set the sampler to the correct texture unit
			objShader.setInt("material.diffuse", 0);
			objShader.setInt("material.specular", 1);
			objShader.setFloat("material.shininess", 32.0f);

			///////////////////////////////////
			// light properties
			// directional light
			objShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			objShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			objShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			objShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

			// point lights
			for (int i = 0; i < s_pointLightNumber; ++i)
			{
				std::stringstream ss;
				ss << "pointLights[" << i << "].";
				string keyPrefix = ss.str();

				objShader.setVec3(keyPrefix + "pos", s_pointLightPositions[i]);
				objShader.setVec3(keyPrefix + "ambient", 0.05f);
				//objShader.setVec3(keyPrefix + "diffuse", 0.8f);
				objShader.setVec3(keyPrefix + "diffuse", s_pointLightColor[i]);
				objShader.setVec3(keyPrefix + "specular", 1.0f);
				objShader.setFloat(keyPrefix + "constant", 1.0f);
				objShader.setFloat(keyPrefix + "linear", 0.09f);
				objShader.setFloat(keyPrefix + "quadratic", 0.032f);
			}

			//spot light
			objShader.setVec3("spotLight.pos", camera.pos);
			objShader.setVec3("spotLight.direction", camera.forward);
			objShader.setVec3("spotLight.ambient", 0.0f);
			objShader.setVec3("spotLight.diffuse", 1.0f);
			objShader.setVec3("spotLight.specular", 1.0f);
			objShader.setFloat("spotLight.constant", 1.0f);
			objShader.setFloat("spotLight.linear", 0.09f);
			objShader.setFloat("spotLight.quadratic", 0.032f);
			objShader.setFloat("spotLight.cutoffCos", glm::cos(glm::radians(12.5f)));
			objShader.setFloat("spotLight.outerCutoffCos", glm::cos(glm::radians(15.0f)));

			glm::mat4 objMatrixM = glm::mat4(1.0f);
			glm::mat4 objMatrixMVP = matrixP * matrixV * objMatrixM;
			objShader.setMat4("MVP", objMatrixMVP);
			objShader.setMat4("M", objMatrixM);

			diffuseMap.bind(GL_TEXTURE0);
			specularMap.bind(GL_TEXTURE1);

			glBindVertexArray(vaoCube);
			for (int i = 0; i < 10; ++i)
			{
				objMatrixM = glm::mat4(1.0f);
				objMatrixM = glm::translate(objMatrixM, s_cubePositions[i]);
				float angle = 20.0f * i;
				objMatrixM = glm::rotate(objMatrixM, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

				objMatrixMVP = matrixP * matrixV * objMatrixM;
				objShader.setMat4("MVP", objMatrixMVP);
				objShader.setMat4("M", objMatrixM);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// draw the lamp
			lampShader.use();
			glBindVertexArray(vaoLight);
			for (unsigned int i = 0; i < s_pointLightNumber; ++i)
			{
				glm::mat4 lampMatrixM = glm::translate(M::i, s_pointLightPositions[i]);
				lampMatrixM = glm::scale(lampMatrixM, glm::vec3(0.2f));
				glm::mat4 lampMatrixMVP = matrixP * matrixV * lampMatrixM;
				lampShader.setMat4("MVP", lampMatrixMVP);
				lampShader.setVec3("color", s_pointLightColor[i]);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glBindVertexArray(0);
		}
		
		// swap buffers and poll IO events(keys pressed/released, mouse moved etc.)
		// ------------------------------
		{
			// check and call events and swap the buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	// optional: de-allocate all resources once they've outlived their purpose.
	glDeleteVertexArrays(1, &vaoCube);
	glDeleteBuffers(1, &vbo);

	// glfw: terminate, clearing all previously allocated GLFW resource.
	glfwTerminate();

	return 0;
}


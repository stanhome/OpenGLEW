#include <iostream>

#include "mainWrapper.h"

using namespace std;

static const GLfloat s_vertices[] = {
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

static const GLuint s_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3
};

const char *SHADER_VERTEX_PATH = "res/shaders/01_started/1-5-matrix.vs";
const char *SHADER_FREGMENT_PATH = "res/shaders/01_started/1-5-matrix.fs";

const int WIDTH = 800;
const int HEIGHT = 600;

glm::vec3 s_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 s_cameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 s_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void onProcessInput(GLFWwindow *window)
{
	const float CAMERA_SPEED = 0.05f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		s_cameraPos += CAMERA_SPEED * s_cameraForward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		s_cameraPos -= CAMERA_SPEED * s_cameraForward;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		s_cameraPos -= glm::normalize(glm::cross(s_cameraForward, s_cameraUp)) * CAMERA_SPEED;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		s_cameraPos += glm::normalize(glm::cross(s_cameraForward, s_cameraUp)) * CAMERA_SPEED;
}

int main()
{
	s_processInputFunc = &onProcessInput;

	GLFWwindow *window = createWindow(WIDTH, HEIGHT);
	if (window == nullptr) return -1;

	Shader myShader(SHADER_VERTEX_PATH, SHADER_FREGMENT_PATH);

	// generate render object
	// ebo: // element buffer object, or IBO index buffer object
	GLuint vbo, ebo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// 1. bind Vertex Array Object
	// bind the vertex array object first, then bind and set vertex buffers and attribute pointer
	glBindVertexArray(vao);
	{
		// copy our vertices array in a buffer for OpenGL to use.
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		/*
		The fourth parameter specifies how we want the graphics card to manage the given data.
		GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		GL_STATIC_DRAW: the data is set only once and used many times.
		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
		*/
		glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_indices), s_indices, GL_STATIC_DRAW);

		//set the vertex attributes pointers.
		// STRIDE: 8 * sizeof(GLfloat)
		int stride = 5 * sizeof(GLfloat);
		// pos attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//// color attribute
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));
		//glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	}
	// 4.  Unbind VAO  Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)  remember: do NOT unbind the EBO, keep it bound to this VAO
	glBindVertexArray(0);

	// draw in wireframe mode.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// load and create a texture
	Texture tex1(GL_TEXTURE_2D, "res/imgs/container.jpg", GL_RGB);
	Texture tex2(GL_TEXTURE_2D, "res/imgs/awesomeface.png", GL_RGBA);

	// tell OpenGL for each sampler to which texture unit it belongs to (only has to be done onece)
	myShader.use(); // don't forget to activate/use the shader before setting uniforms.
	myShader.setInt("texture1", 0);
	// or set it manually like so:
	glUniform1i(glGetUniformLocation(myShader.id, "texture2"), 1);


	glm::vec3 cubePositions[] = {
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

	glm::mat4 matrixM = glm::mat4(1.0f);
	glm::mat4 matrixV = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move.
	matrixV = glm::translate(matrixV, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 matrixP;
	matrixP = glm::perspective(glm::radians(45.0f), WIDTH * 1.0f/ HEIGHT, 0.1f, 100.0f);
	
	// column vector, so multiplication from right to left.
	glm::mat4 matrixMVP = matrixP * matrixV * matrixM;
	glm::mat4 matrixVP = matrixP * matrixV;

	// RENDER loop
	while (!glfwWindowShouldClose(window)) {
		//input
		processInput(window);

		//Rendering commands here
		{
			// do Rendering
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// bind textures on corresponding texture units
			tex1.bind(GL_TEXTURE0);
			tex2.bind(GL_TEXTURE1);

			//glUseProgram(s_shaderProgram);
			myShader.use();

			//matrixM

			// Get matrix's uniform location and set matrix
			GLint mvpLoc = glGetUniformLocation(myShader.id, "MVP");
			//glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(matrixMVP));

			// Draw container
			glBindVertexArray(vao);

			// rotation camera test
			const float radius = 10.0f;
			float time = glfwGetTime();
			float camX = sin(time) * radius;
			float camZ = cos(time) * radius;
			matrixV = glm::lookAt(glm::vec3(camX, 0, camZ), V::zero, V::up);

			for (int i = 0; i < 10; ++i)
			{
				matrixM = glm::translate(glm::mat4(1.0f), cubePositions[i]);
				float angle = 20.0f * i;
				matrixM = glm::rotate(matrixM, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				//matrixMVP = matrixVP * matrixM;
				matrixMVP = matrixP * matrixV * matrixM;
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &matrixMVP[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		
		{
			// check and call events and swap the buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	// optional: de-allocate all resources once they've outlived their purpose.
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	// glfw: terminate, clearing all previously allocated GLFW resource.
	glfwTerminate();

	return 0;
}


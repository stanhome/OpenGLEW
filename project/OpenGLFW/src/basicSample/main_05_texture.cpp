#include <iostream>

#include "mainWrapper.h"

using namespace std;

static const GLfloat s_vertices[] = {
	// positions          // colors           // texture coords
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

static const GLuint s_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3
};

const char *SHADER_VERTEX_PATH = "res/shaders/01_started/1-4-texture.vs";
const char *SHADER_FREGMENT_PATH = "res/shaders/01_started/1-4-texture.fs";


int main()
{
	GLFWwindow *window = createWindow(800, 600);
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
		int stride = 8 * sizeof(GLfloat);
		// pos attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	}
	// 4.  Unbind VAO  Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)  remember: do NOT unbind the EBO, keep it bound to this VAO
	glBindVertexArray(0);

	// draw in wireframe mode.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// load and create a texture
	GLuint texture1, texture2;
	// texture1
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GLREPEAT also is default wrapping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, channel;
	// tel stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("res/imgs/container.jpg", &width, &height, &channel, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "[E] Failed to load texture" << endl;
	}
	stbi_image_free(data);


	// RENDER loop
	while (!glfwWindowShouldClose(window)) {
		//input
		processInput(window);

		//Rendering commands here
		{
			// do Rendering
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// bind textures on corresponding texture units
			//glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);

			//glUseProgram(s_shaderProgram);
			myShader.use();

			glBindVertexArray(vao);
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		
		{
			// check and call events and swap the buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();

	return 0;
}


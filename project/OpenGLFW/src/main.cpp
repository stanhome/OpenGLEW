#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static const float s_halfSize = 1.0f;

static const GLfloat s_vertices[] = {
	-s_halfSize, -s_halfSize, 0.0f,
	s_halfSize, -s_halfSize, 0.0f,
	-s_halfSize,  s_halfSize, 0.0f,

	-s_halfSize, s_halfSize, 0.0f,
	s_halfSize, -s_halfSize, 0.0f,
	s_halfSize,  s_halfSize, 0.0f,
};

const char *SHADER_VERTEX = R"(
#version 430 core 
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

const char *SHADER_FRAGMENT = R"(
#version 430 core
#extension GL_NV_shader_thread_group : require

uniform uint  gl_WarpSizeNV;	// the total number of thread in a warp
uniform uint  gl_WarpsPerSMNV;	// the maximum number of warp executing on a SM
uniform uint  gl_SMCountNV;		// the number of SM on the GPU

in uint  gl_WarpIDNV;		// hold the warp id of the executing thread
in uint  gl_SMIDNV;			// hold the SM id of the executing thread，range [0, gl_SMCountNV - 1]
in uint  gl_ThreadInWarpNV;	// hold the id of the thread within the thread group(or warp)，range [0, gl_WarpSizeNV - 1]

out vec4 FragColor;
void main()
{
    // SM id
	//float SMCountNV = gl_SMCountNV;
	//float lightness = gl_SMIDNV / SMCountNV;
	//FragColor = vec4(lightness, 1);
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
//void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


GLuint s_shaderProgram;
void buildShaders()
{
	//.vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &SHADER_VERTEX, NULL);
	glCompileShader(vertexShader);

	auto checkIsSuccessFunc = [](unsigned int shaderId, bool isSuccess, const char *title) -> void {
		if (! isSuccess)
		{
			GLchar inforLog[512];
			glGetShaderInfoLog(shaderId, 512, NULL, inforLog);
			std::cout << "[E] " << title << " failed:\n" << inforLog << std::endl;
		}
	};

	GLint isSuccess;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isSuccess);
	checkIsSuccessFunc(vertexShader, isSuccess, "vertex compilation");

	// fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &SHADER_FRAGMENT, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isSuccess);
	checkIsSuccessFunc(fragmentShader, isSuccess, "fragment compilation");

	// link shader program
	s_shaderProgram = glCreateProgram();
	glAttachShader(s_shaderProgram, vertexShader);
	glAttachShader(s_shaderProgram, fragmentShader);
	glLinkProgram(s_shaderProgram);

	glGetProgramiv(s_shaderProgram, GL_LINK_STATUS, &isSuccess);
	checkIsSuccessFunc(s_shaderProgram, isSuccess, "Shader program Link");

	glUseProgram(s_shaderProgram);
	//don't forget to delete the shader objects once we've linked them into the program object; we no longer need them anymore:
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// 锟斤拷锟斤拷GLFW锟斤拷锟斤拷使锟矫碉拷锟角猴拷锟斤拷模式(Core-profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// 锟斤拷锟绞癸拷玫锟斤拷锟組ac OS X系统锟斤拷锟姐还锟斤拷要锟斤拷锟斤拷锟斤拷锟斤拷锟叫达拷锟诫到锟斤拷某锟绞硷拷锟斤拷锟斤拷锟斤拷锟斤拷锟叫╋拷锟斤拷貌锟斤拷锟斤拷锟斤拷锟斤拷茫锟?
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return -1;
	}

	glfwMakeContextCurrent(window);

	// init GLAD
	// GLFW gives us glfwGetProcAddress that defines the correct function based on which OS we're compiling for
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//glfwSetKeyCallback(window, keyCallback);

	buildShaders();

	// generate render object
	unsigned int vbo;
	glGenBuffers(1, &vbo);

	GLuint vao;
	glGenVertexArrays(1, &vao);

	// 1. bind Vertex Array Object
	glBindVertexArray(vao);
	{
		// 2. copy our vertices array in a buffer for OpenGL to use.
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		/*
		The fourth parameter specifies how we want the graphics card to manage the given data.
		GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		GL_STATIC_DRAW: the data is set only once and used many times.
		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
		*/
		glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);

		// 3. set the vertex attributes pointers.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	}
	// 4.  Unbind VAO  Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
	glBindVertexArray(0);


	// RENDER loop
	while (!glfwWindowShouldClose(window)) {
		//input
		processInput(window);

		//Rendering commands here
		{
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// do Rendering
			glUseProgram(s_shaderProgram);
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
		
		{
			// check and call events and swap the buffers
			glfwPollEvents();
			glfwSwapBuffers(window);
		}
	}

	glfwTerminate();

	return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

// key event method 2.
//void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}



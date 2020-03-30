#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static const GLfloat s_vertices[] = {
	0.5f,  0.5f, 0.0f,  // top right
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};

static const GLuint s_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};

const char *SHADER_VERTEX = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *SHADER_FRAGMENT = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// 告诉GLFW我们使用的是核心模式(Core-profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// 如果使用的是Mac OS X系统，你还需要加下面这行代码到你的初始化代码中这些配置才能起作用：
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	}
	// 4.  Unbind VAO  Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)  remember: do NOT unbind the EBO, keep it bound to this VAO
	glBindVertexArray(0);

	// draw in wireframe mode.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


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
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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



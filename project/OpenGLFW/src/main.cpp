#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
//void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// ���ʹ�õ���Mac OS Xϵͳ���㻹��Ҫ���������д��뵽��ĳ�ʼ����������Щ���ò��������ã�
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

	// RENDER loop
	while (!glfwWindowShouldClose(window)) {
		//input
		processInput(window);

		//Rendering commands here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
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



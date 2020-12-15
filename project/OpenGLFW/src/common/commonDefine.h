#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class V {
public:
	static const glm::vec3 zero;
	static const glm::vec3 left;
	static const glm::vec3 up;
	static const glm::vec3 forward;
};


class M {
public:
	static const glm::mat4 i;
};



static float s_quadVertices[] = {
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};


static GLenum __glCheckError(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			default: error = "Unknown GL_ERROR code:" + std::to_string(errorCode);
		}

		std::cout << "[GL_E] " << error << "(" << file << ":" << line << ")" << std::endl;
	}

	return errorCode;
}

#define glCheckError() __glCheckError(__FILE__, __LINE__);
void glad_post_call_callback(const char *name, void *funcptr, int len_args, ...);


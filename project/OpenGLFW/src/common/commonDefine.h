#pragma once
#include <iostream>
#include <glm/glm.hpp>

class V {
public:
	static const glm::vec3 zero;
	static const glm::vec3 left;
	static const glm::vec3 up;
	static const glm::vec3 forward;
};

const glm::vec3 V::zero = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 V::left = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 V::up = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 V::forward = glm::vec3(0.0f, 0.0f, 1.0f);

class M {
public:
	static const glm::mat4 i;
};

const glm::mat4 M::i = glm::mat4(1.0f);


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


GLenum __glCheckError(const char *file, int line) {
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


void glad_post_call_callback(const char *name, void *funcptr, int len_args, ...) {
	GLenum error_code;

	(void)funcptr;
	(void)len_args;

	error_code = glad_glGetError();

	if (error_code != GL_NO_ERROR) {
		std::string error;
		switch (error_code)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default: error = "Unknown GL_ERROR code";
		}

		fprintf(stderr, "ERROR %d %s in %s\n", error_code, error.c_str(), name);
	}
}


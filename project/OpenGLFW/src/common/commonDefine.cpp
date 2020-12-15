#include "commonDefine.h"

const glm::vec3 V::zero = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 V::left = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 V::up = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 V::forward = glm::vec3(0.0f, 0.0f, 1.0f);

const glm::mat4 M::i = glm::mat4(1.0f);

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


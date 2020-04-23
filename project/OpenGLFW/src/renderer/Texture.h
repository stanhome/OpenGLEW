#pragma once

#include <string>

#include <glad/glad.h>

class Texture
{
public:
	/*
	 * @param target: texture target like GL_TEXTURE_2D
	 * @param format: texture format like GL_RGBA
	 */
	Texture(GLenum target, const std::string &filePath);

	static void initStbi();

	// @param textureUnit, like: GL_TEXTURE0
	void bind(GLenum textureUnit);

	inline void setSamplerName(const std::string &namePrefix, int idx) {
		_samplerName = namePrefix + std::to_string(idx);
	}
	inline const std::string &getSamplerName() const {
		return _samplerName;
	}

private:
	void load(const std::string &filePath);

public:

private:
	GLenum _target;
	GLuint _textureObj;

	std::string _samplerName;
};

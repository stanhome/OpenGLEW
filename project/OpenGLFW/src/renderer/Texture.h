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
	Texture(GLenum target, const std::string &filePath, GLenum format);

	static void initStbi();

	// @param textureUnit, like: GL_TEXTURE0
	void bind(GLenum textureUnit);
private:
	void load(const std::string &filePath);

private:
	GLenum _target;
	GLenum _format;
	GLuint _textureObj;
};

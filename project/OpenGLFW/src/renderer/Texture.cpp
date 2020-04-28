#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <io.h> // for C

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture.h"

using namespace std;

void Texture::initStbi()
{
	// tel stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);
}

Texture::Texture(GLenum target, const std::string &filePath, GLint wrap /*= GL_REPEAT*/)
: _target(target)
, _wrap(wrap)
{
	load(filePath);
}

void Texture::load(const std::string &filePath)
{
#if _LOG_ == 1
	cout << "Texture::load begin, path:" << filePath << endl;
#endif

	glGenTextures(1, &_textureObj);
	glBindTexture(_target, _textureObj);
	// set the texture wrapping parameters
	glTexParameteri(_target, GL_TEXTURE_WRAP_S, _wrap); //GL_REPEAT also is default wrapping method
	glTexParameteri(_target, GL_TEXTURE_WRAP_T, _wrap);
	// set texture filtering parameters
	glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, channel;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &channel, 0);
	if (data)
	{
		GLenum format;
		switch (channel)
		{
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default : break;
		}

		glTexImage2D(_target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(_target);
	}
	else
	{
		cout << "[E] Failed to load texture" << endl;
	}
	stbi_image_free(data);
#if _LOG_ == 1
	cout << "Texture::load end." << endl;
#endif
}

void Texture::bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(_target, _textureObj);
}
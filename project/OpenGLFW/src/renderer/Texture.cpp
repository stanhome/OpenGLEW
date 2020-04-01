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

Texture::Texture(GLenum target, const std::string &filePath, GLenum format)
: _target(target)
, _format(format)
{
	load(filePath);
}

void Texture::load(const std::string &filePath)
{
	glGenTextures(1, &_textureObj);
	glBindTexture(_target, _textureObj);
	// set the texture wrapping parameters
	glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_REPEAT); //GLREPEAT also is default wrapping method
	glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, channel;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &channel, 0);
	if (data)
	{
		glTexImage2D(_target, 0, _format, width, height, 0, _format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(_target);
	}
	else
	{
		cout << "[E] Failed to load texture" << endl;
	}
	stbi_image_free(data);
}

void Texture::bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(_target, _textureObj);
}
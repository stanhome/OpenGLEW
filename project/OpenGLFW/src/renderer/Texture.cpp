#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <io.h> // for C

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture.h"

using namespace std;


struct ImageData {
public:
	int width = 0, height = 0, channel = 0;
	bool isHDR = false;
	void *data = nullptr;

	inline bool isValid() const { return data != nullptr; }

	inline ImageData(const std::string &filePath) {
		isHDR = filePath.find_last_of(".hdr") != std::string::npos;
		if (isHDR) {
			data = stbi_loadf(filePath.c_str(), &width, &height, &channel, 0);
		}
		else {
			data = stbi_load(filePath.c_str(), &width, &height, &channel, 0);
		}
	}

	inline ~ImageData() {
		stbi_image_free(data);
	}
};


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

	do 
	{
		const ImageData data(filePath);

		if (! data.isValid()) {
			cout << "[E] Failed to load texture:" << filePath << endl;
			break;
		}

		if (data.isHDR)
		{
			// hdr format
			glTexImage2D(_target, 0, GL_RGB16F, data.width, data.height, 0, GL_RGB, GL_FLOAT, data.data);
			break;
		}

		// common format
		GLenum format;
		switch (data.channel)
		{
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default: break;
		}

		glTexImage2D(_target, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.data);
		glGenerateMipmap(_target);

		break;
	} while (true);

#if _LOG_ == 1
	cout << "Texture::load end." << endl;
#endif
}

void Texture::bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(_target, _textureObj);
}
/*************************************************
Author: Stan
Created: 2019-6-12
Note: for debug info display in Screen.
************************************************/
#include "ShowTextureAtlas.h"

#include <glad/glad.h>
#include "common/log.h"
#include "common/utils.h"
#include "common/commonDefine.h"

namespace show {
//text image data
// 2 x 2 Image, 3bytes per pixel(R, G, B)
//static GLubyte s_pixels[4 * 3] =
//{
//	255,	0,		0,		//Red
//	0,		255,	0,		//Green
//	0,		0,		255,	//Blue
//	255,	255,	0		//Yellow
//};


static GLint convertTGAFormatToGL(tImageTGA* tga)
{
	GLint ret = GL_RGBA; // by default

	do
	{
		//only support 2, 10
		if (tga->type == 2 || tga->type == 10)
		{
			if (tga->pixelDepth == 32)
			{
				//RGBA8888
				ret = GL_RGBA;
				break;
			}
		}

		XLOGE("unsupported tga data pixel format(type:%d, pixel depth:%d, %dx%d)", tga->type, tga->pixelDepth, tga->width, tga->height);
	} while (false);

	return ret;
}

STextureAtlas::STextureAtlas(const std::string &file, size_t capacity)
: _capacity(capacity)
{
	loadImageData(file);
	init();
}

STextureAtlas::~STextureAtlas()
{
	glDeleteBuffers(2, _buffersVBO);
	glDeleteTextures(1, &_textureId);
}

void STextureAtlas::init()
{
	if (_img == NULL)
		return;
	loadTextureToGL();

	_quads = (V2F_T2F_Quad *)malloc(_capacity * sizeof(V2F_T2F_Quad));
	_indices = (GLushort *)malloc(_capacity * 6 * sizeof(GLushort));

	memset(_quads, 0, _capacity * sizeof(V2F_T2F_Quad));
	memset(_indices, 0, _capacity * 6 * sizeof(GLushort));

	setupIndices();

	setupVBO();

	_isDirty = true;
}

void STextureAtlas::loadImageData(const std::string &file)
{
	std::string fullPath = getFullPath(file.c_str());

	FILE *f = fopen(fullPath.c_str(), "rb");
	fseek(f, 0L, SEEK_END);
	int len = ftell(f);
	unsigned char *buf = new unsigned char[len];
	memset(buf, 0, len);
	
	rewind(f);
	fread(buf, 1, len, f);
	fclose(f);

	_img = tgaLoadBuffer(buf, len);

	_w = _img->width;
	_h = _img->height;

	delete[] buf;
}

void STextureAtlas::loadTextureToGL()
{
	GLint tgaFormatForGL = convertTGAFormatToGL(_img);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &_textureId);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, s_pixels);
	glTexImage2D(GL_TEXTURE_2D, 0, tgaFormatForGL, (GLsizei)_img->width, (GLsizei)_img->height, 0, tgaFormatForGL, GL_UNSIGNED_BYTE, _img->imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	tgaDestroy(_img);
	_img = nullptr;
}

void STextureAtlas::setupVBO()
{
	glGenVertexArrays(1, &_vao);
	glGenBuffers(2, &_buffersVBO[0]);

	mapBuffers();
}

void STextureAtlas::mapBuffers()
{
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _capacity, _quads, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _capacity * 6, _indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(_quads[0].tl), (GLvoid *)offsetof(V2F_T2F, vertices));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(_quads[0].tl), (GLvoid *)offsetof(V2F_T2F, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void STextureAtlas::bindTexture()
{
	glActiveTexture(GL_TEXTURE0 + _textureUnit);
	glBindTexture(GL_TEXTURE_2D, _textureId);
}



void STextureAtlas::setupIndices()
{
	for (unsigned short i = 0; i < _capacity; i++)
	{
		_indices[i * 6 + 0] = i * 4 + 0;
		_indices[i * 6 + 1] = i * 4 + 1;
		_indices[i * 6 + 2] = i * 4 + 2;

		_indices[i * 6 + 3] = i * 4 + 3;
		_indices[i * 6 + 4] = i * 4 + 2;
		_indices[i * 6 + 5] = i * 4 + 1;
	}
}

void STextureAtlas::resizeCapacity(size_t newCapacity)
{
	if (newCapacity == _capacity)
	{
		return;
	}

	auto oldCapacity = _capacity;
	_totalQuads = glm::min(_totalQuads, newCapacity);
	_capacity = newCapacity;

	// reset quads memory
	V2F_T2F_Quad *tmpQuads = nullptr;
	size_t quadSize = sizeof(_quads[0]);
	size_t newQuadArraySize = _capacity * quadSize;
	if (_quads = nullptr)
	{
		tmpQuads = (V2F_T2F_Quad *)malloc(newQuadArraySize);
		if (tmpQuads != nullptr)
		{
			memset(tmpQuads, 0, newQuadArraySize);
		}
	}
	else
	{
		tmpQuads = (V2F_T2F_Quad *)realloc(_quads, newQuadArraySize);
		if (tmpQuads != nullptr && _capacity > oldCapacity)
		{
			memset(tmpQuads + oldCapacity, 0, (_capacity - oldCapacity) * quadSize);
		}

		_quads = nullptr;
	}

	// reset indices memory
	GLushort *tmpIndices = nullptr;
	size_t indexSize = sizeof(_indices[0]);
	size_t newIndexArraySize = _capacity * 6 * indexSize;

	if (_indices == nullptr)
	{
		tmpIndices = (GLushort *)malloc(newIndexArraySize);
		if (tmpIndices)
		{
			memset(tmpIndices, 0, newIndexArraySize);
		}
	}
	else
	{
		tmpIndices = (GLushort *)realloc(_indices, newIndexArraySize);
		if (tmpIndices && _capacity > oldCapacity)
		{
			memset(tmpIndices + oldCapacity, 0, (_capacity - oldCapacity) * 6 * indexSize);
		}

		_indices = nullptr;
	}

	_quads = tmpQuads;
	_indices = tmpIndices;

	setupIndices();
	mapBuffers();

	_isDirty = true;
}

/////////////////////////////////////////////////////////////
// public method

void STextureAtlas::increaseTotalQuadsWith(size_t amount)
{
	_totalQuads += amount;
}

void STextureAtlas::drawQuads()
{
	drawQuads(0, _totalQuads);
}

void STextureAtlas::drawQuads(size_t start, size_t count)
{
	if (count == 0)
	{
		return;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind texture
	bindTexture();


	glBindVertexArray(_vao);
	// bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
	if (_isDirty)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(_quads[0]) * _totalQuads, &_quads[0]);
		_isDirty = false;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);

	glDrawElements(GL_TRIANGLES, count * 6, GL_UNSIGNED_SHORT, (GLvoid *)(start * 6 * sizeof(_indices[0])));
	glBindVertexArray(0);


	// clean up
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

V2F_T2F_Quad *STextureAtlas::getQuads()
{
	_isDirty = true;
	return _quads;
}



}
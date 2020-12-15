/*************************************************
Author: Stan
Created: 2019-6-12
Note: for debug info display in Screen.
************************************************/
#pragma once
#include <string>
#include "ShowComman.h"
#include "stypes.h"
#include "tgalib.h"


namespace show {

class STextureAtlas {
public:
	STextureAtlas(const std::string &file, size_t capacity);
	~STextureAtlas();

	inline GLuint getTextureUnit() { return _textureUnit; }
	inline GLuint getTextureID() { return _textureId; }

	inline GLuint getTotalQuads() { return _totalQuads; }

	inline void setDirty(bool isDirty) { _isDirty = isDirty; }

	void increaseTotalQuadsWith(size_t amount);

	inline int getW() { return _w; }
	inline int getH() { return _h; }

	void drawQuads();
	void drawQuads(size_t start, size_t count);
	V2F_T2F_Quad *getQuads();

	void resizeCapacity(size_t capacity);

private:
	void init();
	void loadImageData(const std::string &file);
	void loadTextureToGL();

	void setupVBO();
	void setupIndices();
	void mapBuffers();

	void bindTexture();


private:
	// texture
	tImageTGA *_img = nullptr;
	int _w = 0;
	int _h = 0;
	GLuint _textureId = 0;
	GLuint _textureUnit = 0;

	// vbo
	GLushort *_indices = nullptr;
	GLuint _vao = 0;
	GLuint _buffersVBO[2]; //0: vertex 1: indices
	// indicates whether or not the array buffer of the VBO needs to be updated.
	bool _isDirty = true;

	size_t _totalQuads = 0;
	size_t _capacity = 0;

	// Quads that are going to be rendered.
	V2F_T2F_Quad *_quads = nullptr;
};

}
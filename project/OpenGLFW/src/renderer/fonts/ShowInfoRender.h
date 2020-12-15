/*************************************************
Author: Stan
Created: 2019-6-4
Note: for debug info display in Screen.
************************************************/

#pragma once
#include "ShowComman.h"
#include "ShowTextureAtlas.h"
#include "renderer/Shader.h"

namespace show {

class ShowInfoRender {
public:
	/*
	 * @ tagImagePath, tag image path. from engine root path.
	 * @ mapStartChar, atlas font, first char ASSIC code
	 * @ tileWidth, one tile width
	 * @ tileHeight, one tile height
	 * @ margin, margin between tile
	 * @ tileCountPerRow, tile count per row
	 * @ tileCountPerColumn, tile count per column
	 * @ quadsToDrawCount, 需要绘制多少个 tile(一个 tile 通过一个 quad 来渲染)，即需要绘制几个字，当调用 setContent 的文字数量超过这个值时，这个值会随着增加。
	 * 
	*/
	bool init(const std::string &tgaImagePath, unsigned char mapStartChar, int tileWidth, int tileHeight, int margin, size_t tileCountPerRow, size_t tileCountPreColumn, size_t quadsToDrawCount);
	void render();
	void destroy();
	void onScreenChangeSize(float w, float h);

	void setContent(const std::string &content);

	void setPosition(float x, float y);
	void setScale(float s);
	void setAnchor(float x, float y);

private:
	void setNodeSize(const Size &&s);

	void updateAtlasValues();
	size_t updateAtlasValuesOneLine(const std::string &line, size_t lineIndx, V2F_T2F_Quad *&curQuad);
	void updateMatrixP();

private:
	STextureAtlas *_texture = nullptr;

	size_t _tilePerRow = 0;
	size_t _tilePerColumn = 0;
	int _tileWidth = 0;
	int _tileHeight = 0;
	int _margin = 0;

	float _matrixP[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	float _isMatrixDirty = true;
	float _scale = 1.0f;
	Vec2 _pos = Vec2();
	Size _screenSize = Size();
	Vec2 _anchor = Vec2(0, 1.0f);

	// content to render
	std::string _content = std::string();
	// the first char in the char map
	unsigned char _mapStartChar = 0;

	size_t _quadsToDrawCount = 0;
	Size _nodeSize = Size();
};

}

/*************************************************
Author: Stan
Created: 2019-6-4
Note: for debug info display in Screen.
************************************************/
#include <vector>
#include <sstream>
#include <iterator>
#include "ShowInfoRender.h"
#include "ShowComman.h"
#include "stypes.h"

using namespace std;

namespace show {

//#define MULTILINE(...) #__VA_ARGS__
#define MULTILINE(...) R"(__VA_ARGS__)"

static const char *VS = MULTILINE(
##version 330 core\n
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

uniform mat4 s_matrixP;

void main()
{
	gl_Position = s_matrixP * vec4(a_position.x, a_position.y, 0, 1.0);
	v_texCoord = a_texCoord;
});

static const char *FS = MULTILINE(
##version 330 core\n
in vec2 v_texCoord;

out vec4 FragColor;

uniform sampler2D s_texture;

void main()
{
	FragColor = texture2D(s_texture, vec2(v_texCoord.x, 1.0 - v_texCoord.y));
}
);


const char *SHADER_VERTEX = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *SHADER_FRAGMENT = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

static void addShader(GLuint shaderProgram, const char *pShaderText, GLenum shaderType)
{
	GLuint shaderObj = glCreateShader(shaderType);
	if (shaderObj == 0)
	{
		XLOGE("error creating shader type %d", shaderType);
	}

	const GLchar *p[1];
	p[0] = pShaderText;
	GLint lengths[1];
	lengths[0] = strlen(pShaderText);
	glShaderSource(shaderObj, 1, p, lengths);
	glCompileShader(shaderObj);

	GLint isSuccess;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &isSuccess);
	if (!isSuccess)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderObj, 1024, NULL, infoLog);
		XLOGE("Error compiling shader type %d: %s\n", shaderType, infoLog);
	}

	glAttachShader(shaderProgram, shaderObj);
	glDeleteShader(shaderObj);
}

GLuint s_shaderProgram;
GLint s_samplerLoc;
GLint s_matrixPLoc;

static void compileShaders()
{
	GLuint shaderProgram = glCreateProgram();

	addShader(shaderProgram, VS, GL_VERTEX_SHADER);
	addShader(shaderProgram, FS, GL_FRAGMENT_SHADER);

	GLint isSuccess = 0;
	GLchar errorLog[1024] = {0};

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isSuccess);
	if (isSuccess == 0)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL ,errorLog);
		XLOGE("Error linking shader program: %s\n", errorLog);
	}

	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &isSuccess);
	if (! isSuccess)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), NULL, errorLog);
		XLOGE("Invalid shader program: %s\n", errorLog);
	}

	s_shaderProgram = shaderProgram;
}


bool ShowInfoRender::init(const std::string &tgaImagePath, unsigned char mapStartChar, int tileWidth, int tileHeight, int margin, size_t tileCountPerRow, size_t tileCountPreColumn, size_t quadsToDrawCount)
{
	XLOG("ShowInfoRender::init begin");

	_mapStartChar = mapStartChar;
	_tileWidth = tileWidth;
	_tileHeight = tileHeight;
	_margin = margin;
	_tilePerRow = tileCountPerRow;
	_tilePerColumn = tileCountPreColumn;

	_quadsToDrawCount = quadsToDrawCount;
	_texture = new STextureAtlas(tgaImagePath, _quadsToDrawCount);

	//_s = new Shader(SHADER_VERTEX, SHADER_FRAGMENT, true);
	//_s->use();
	//_s->setInt("s_texture", _texture->getTextureUnit());

	// init shader
	compileShaders();
	glUseProgram(s_shaderProgram);
	s_samplerLoc = glGetUniformLocation(s_shaderProgram, "s_texture");
	if (s_samplerLoc == -1)
	{
		XLOGE("[e]can't find uniform location %s in shader program.\n", "s_texture");
	}

	glUniform1i(s_samplerLoc, _texture->getTextureUnit());

	s_matrixPLoc = glGetUniformLocation(s_shaderProgram, "s_matrixP");
	if (s_matrixPLoc == -1)
	{
		XLOGE("[e]can't find uniform location %s in shader program.\n", "s_matrixP");
	}

	return true;
}

void ShowInfoRender::render()
{
	if (_texture->getTotalQuads() == 0 || _texture->getTextureID() == 0)
		return;
	glUseProgram(s_shaderProgram);

	if (_isMatrixDirty)
	{
		updateMatrixP();
		_isMatrixDirty = false;
		glUniformMatrix4fv(s_matrixPLoc, 1, GL_FALSE, _matrixP);
	}

	_texture->drawQuads();
}

void ShowInfoRender::onScreenChangeSize(float w, float h)
{
	_isMatrixDirty = true;
	_screenSize.width = w;
	_screenSize.height = h;
}

void ShowInfoRender::destroy()
{
	glDeleteProgram(s_shaderProgram);

	if (_texture)
	{
		delete _texture;
		_texture = nullptr;
	}
}

void ShowInfoRender::setContent(const std::string & content)
{
	size_t len = content.size();
	if (len > _texture->getTotalQuads())
	{
		_texture->resizeCapacity(len);
	}
	_content = content;

	updateAtlasValues();

	_quadsToDrawCount = len;
}

void ShowInfoRender::setPosition(float x, float y)
{
	_isMatrixDirty = true;
	_pos.x = x;
	_pos.y = y;
}

void ShowInfoRender::setScale(float s)
{
	_isMatrixDirty = true;
	_scale = s;
}

void ShowInfoRender::setAnchor(float x, float y)
{
	_isMatrixDirty = true;
	_anchor.x = x;
	_anchor.y = y;
}

void ShowInfoRender::setNodeSize(const Size &&s)
{
	_isMatrixDirty = true;
	_nodeSize = s;
}

void ShowInfoRender::updateAtlasValues()
{
	if (_quadsToDrawCount == 0)
	{
		return;
	}

	size_t quadToDrawCount = 0;
	vector<string> lines;
	stringstream ss(_content);
	std::string token;
	while (std::getline(ss, token))
	{
		lines.push_back(token);
	}

	V2F_T2F_Quad *quads = _texture->getQuads();

	size_t lineIdx = 0;
	size_t maxCol = 0;
	for (vector<string>::reverse_iterator lineIter = lines.rbegin(); lineIter != lines.rend(); ++lineIter)
	{
		quadToDrawCount += updateAtlasValuesOneLine(*lineIter, lineIdx, quads);
		lineIdx++;

		maxCol = glm::max(maxCol, lineIter->length());
	}

	setNodeSize(Size(maxCol * _tileWidth, lines.size() * _tileHeight));

	if (quadToDrawCount > 0)
	{
		_texture->setDirty(true);
		size_t totalQuads = _texture->getTotalQuads();
		if (quadToDrawCount > totalQuads)
		{
			_texture->increaseTotalQuadsWith(static_cast<size_t>(quadToDrawCount - totalQuads));
		}
	}
}

size_t ShowInfoRender::updateAtlasValuesOneLine(const std::string &line, size_t charColIdx, V2F_T2F_Quad *&curQuad)
{
	const unsigned char *s = (const unsigned char *)line.c_str();

	float textureW = (float)_texture->getW();
	float textureH = (float)_texture->getH();


	size_t len = line.length();
	size_t charRowIdx = 0;
	size_t quadCount = 0;
	for (size_t i = 0; i < len; i++)
	{
		charRowIdx++;
		if (s[i] == ' ')
		{
			continue;
		}

		// vertices
		curQuad->bl.vertices.x = (float)(charRowIdx * _tileWidth);
		curQuad->bl.vertices.y = (float)((charColIdx)* _tileHeight);
		curQuad->br.vertices.x = (float)((charRowIdx + 1) * _tileWidth);
		curQuad->br.vertices.y = (float)((charColIdx)* _tileHeight);
		curQuad->tl.vertices.x = (float)(charRowIdx * _tileWidth);
		curQuad->tl.vertices.y = (float)((charColIdx + 1) * _tileHeight);
		curQuad->tr.vertices.x = (float)((charRowIdx + 1) * _tileWidth);
		curQuad->tr.vertices.y = (float)((charColIdx + 1) * _tileHeight);

		// UV
		unsigned char a = s[i] - _mapStartChar;
		size_t rowIdx = (a % _tilePerRow);
		size_t colIdx = (a / _tilePerRow);

		float left = (rowIdx * (_tileWidth + _margin)) / textureW;
		float right = left + _tileWidth / textureW;
		float top = (colIdx * (_tileHeight + _margin)) / textureH;
		float bottom = top + _tileHeight / textureH;

		curQuad->tl.texCoords.u = left;
		curQuad->tl.texCoords.v = top;
		curQuad->tr.texCoords.u = right;
		curQuad->tr.texCoords.v = top;
		curQuad->bl.texCoords.u = left;
		curQuad->bl.texCoords.v = bottom;
		curQuad->br.texCoords.u = right;
		curQuad->br.texCoords.v = bottom;

		curQuad++;
		quadCount++;
	}

	return quadCount;
}

void ShowInfoRender::updateMatrixP()
{
	// glUniformMatrix4fv(s_matrixPLoc, 1, GL_TRUE, _matrixP), GL_TRUE 不知道为什么不起作用。这里就手动转置一下吧。
	_matrixP[0] = 2.0f / _screenSize.width * _scale;
	_matrixP[5] = 2.0f / _screenSize.height * _scale;

	_matrixP[12] = -0.9999f + ((_pos.x - _nodeSize.width * _anchor.x) / _screenSize.width) * 2.0f;
	_matrixP[13] = -0.9999f + ((_pos.y - _nodeSize.height * _anchor.y) / _screenSize.height) * 2.0f;
}

}

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef __APPLE__
#else
#include <io.h> // for C
#endif


#include "Shader.h"
#include "common/utils.h"

using namespace std;

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *gemoetryPath /*= nullptr*/)
	: _vertPath(vertexPath)
	, _fragPath(fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	// 直接使用相对目录也可以(放到工作目录下）
	std::string vCode = readShaderSrc(vertexPath);
	std::string fCode = readShaderSrc(fragmentPath);
	if (gemoetryPath == nullptr)
	{
		init(vCode.c_str(), fCode.c_str());
	}
	else {
		_geometryPath = std::string(gemoetryPath);
		std::string gCode = readShaderSrc(gemoetryPath);
		init(vCode.c_str(), fCode.c_str(), gCode.c_str());
	}

}

Shader::Shader(const GLchar *vertexCode, const GLchar *fragmentCode, bool isCode) {
	init(vertexCode, fragmentCode);
}

void Shader::init(const GLchar *vShaderCode, const GLchar *fShaderCode, const GLchar *gShaderCode /* = nullptr*/)
{
	// 2. compile shader code.
	GLuint vertex, fragment, geometry;

	auto checkIsSuccessFunc = [](GLuint shaderObjId, GLenum pname, const std::string &title) -> void {
		// Print compile errors if any
		GLint isSuccess;

		switch (pname)
		{
		case GL_COMPILE_STATUS:
			glGetShaderiv(shaderObjId, pname, &isSuccess);
			break;
		case GL_LINK_STATUS:
			glGetProgramiv(shaderObjId, pname, &isSuccess);
			break;
		default:
			break;
		}


		if (!isSuccess)
		{
			GLchar inforLog[512];
			glGetShaderInfoLog(shaderObjId, 512, NULL, inforLog);
			std::cout << "[E] " << title << " failed:\n" << inforLog << std::endl;
		}
	};

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkIsSuccessFunc(vertex, GL_COMPILE_STATUS, _vertPath + ", shader vertex compilation");

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkIsSuccessFunc(fragment, GL_COMPILE_STATUS, _fragPath + ", shader fragment compilation");

	// geometry shader
	if (gShaderCode != nullptr)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkIsSuccessFunc(geometry, GL_COMPILE_STATUS, _geometryPath + ", shader geometry compilation.");
	}

	// shader program
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	if (gShaderCode != nullptr)
		glAttachShader(id, geometry);
	glLinkProgram(id);
	checkIsSuccessFunc(id, GL_LINK_STATUS, "shader program linking");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (gShaderCode != nullptr)
		glDeleteShader(geometry);
}

void Shader::use()
{
	glUseProgram(id);
}

string Shader::readShaderSrc(const char *path) const
{
	string ret;
	ifstream srcFile;
	// ensure ifstream objects can throw exceptions:
	srcFile.exceptions(ifstream::badbit);
	try
	{
		// 00 -- check file is exist.
#ifdef __APPLE__
		bool isExist = access(path, 0);
#else
		bool isExist = _access(path, 0);
#endif
		if (isExist == -1) {
			cout << "[E] shader file not found, path:" << path << endl;

			return ret;
		}

		// open file
		srcFile.open(path);
		stringstream fileStream;
		// read file's buffer contents into streams
		fileStream << srcFile.rdbuf();

		ret = fileStream.str();
		// close file handles
		srcFile.close();
	}
	catch (ifstream::failure e)
	{
		cout << "[E]shader file not successfully read." << endl;
	}

	return ret;
}

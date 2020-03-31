#pragma once

#include <string>

#include <glad/glad.h>


class Shader
{
public:
	GLuint id;
	Shader(const char *vertexPath, const char *fragmentPath);
	Shader(const GLchar *vertexCode, const GLchar *fragmentCode, bool isCode);

	void use();

	//utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

protected:
	void init(const GLchar *vShaderCode, const GLchar *fShaderCode);

private:
	std::string readShaderSrc(const char *path) const;
};

inline void Shader::setBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
inline void Shader::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);

}

inline void Shader::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
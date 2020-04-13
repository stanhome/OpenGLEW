#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>


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

	void setVec3(const std::string &name, const glm::vec3 &val) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	void setVec3(const std::string &name, float val) const;
	void setMat4(const std::string &name, const glm::mat4 &val) const;

protected:
	void init(const GLchar *vShaderCode, const GLchar *fShaderCode);

private:
	std::string readShaderSrc(const char *path) const;

private:
	std::string _vertPath;
	std::string _fragPath;
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

inline void Shader::setVec3(const std::string &name, const glm::vec3 &val) const {
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &val[0]);
}

inline void Shader::setVec3(const std::string &name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

inline void Shader::setVec3(const std::string &name, float val) const {
	glUniform3f(glGetUniformLocation(id, name.c_str()), val, val, val);
}

inline void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"

struct Vertex 
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	unsigned int vao;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures);

	void draw(Shader shader);

private:
	unsigned int _vbo, _ebo;// ebo is ibo

private:
	// initializes all the buffer objects/arrays
	void setupMesh();
};

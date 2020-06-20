#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"

class PlaneMesh
{
public:
	std::vector<std::shared_ptr<Texture>> textures;
	unsigned int vao;

	enum Type
	{
		pos = 0,
		posNormal,
		posNormalUV,
	};

public:
	PlaneMesh(Type t = Type::pos);
	PlaneMesh(std::shared_ptr<Texture> texture);
	PlaneMesh(std::vector<std::shared_ptr<Texture>> textures);
	~PlaneMesh();

	void draw(Shader shader);

private:
	unsigned int _vbo;

private:
	// initializes all the buffer objects/arrays
	void setupMesh(Type t = Type::pos);
};

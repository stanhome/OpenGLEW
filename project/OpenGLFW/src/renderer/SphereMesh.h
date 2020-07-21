/*
 * @Date: 2020-07-21 09:05:03
 * @LastEditors: Stan
 * @LastEditTime: 2020-07-21 09:06:06
 */ 
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"

class SphereMesh
{
public:
	std::vector<std::shared_ptr<Texture>> textures;
	unsigned int vao;

public:
	SphereMesh();
	SphereMesh(std::shared_ptr<Texture> texture);
	SphereMesh(std::vector<std::shared_ptr<Texture>> textures);
	~SphereMesh();

	void draw(Shader shader);

private:
	unsigned int _vbo, _ebo = 0;
	unsigned int _indexCount = 0;

private:
	// initializes all the buffer objects/arrays
	void setupMesh();
};

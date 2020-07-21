#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "SphereMesh.h"

using namespace std;

SphereMesh::SphereMesh()
{
	setupMesh();
}

SphereMesh::SphereMesh(std::shared_ptr<Texture> texture)
{
	textures.push_back(texture);
	setupMesh();
}

SphereMesh::SphereMesh(std::vector<std::shared_ptr<Texture>> textures)
	: textures(textures)
{
	setupMesh();
}

SphereMesh::~SphereMesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
}

void SphereMesh::draw(Shader shader)
{
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		auto tex = textures[i];

		// set the sampler to the correct texture unit
		shader.setInt(tex->getSamplerName(), i);
		tex->bind(GL_TEXTURE0 + i);
	}

	//draw mesh
	glBindVertexArray(vao);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawElements(GL_TRIANGLE_STRIP, _indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void SphereMesh::setupMesh()
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	const float PI = 3.14159265359;
	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float theta = ySegment * PI;
			float phi = xSegment * 2.0f * PI;
			float xPos = std::sin(theta) * std::cos(phi);
			float yPos = std::cos(theta);
			float zPos = std::sin(theta) * std::sin(phi);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}

	bool isEvenRow = true;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (isEvenRow) // y ==0, y == 2, and so on.
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}

		isEvenRow = !isEvenRow;
	}
	_indexCount = indices.size();

	std::vector<float> data;
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		const glm::vec3 &pos = positions[i];
		data.push_back(pos.x);
		data.push_back(pos.y);
		data.push_back(pos.z);

		// normal
		const glm::vec3 &n = normals[i];
		data.push_back(n.x);
		data.push_back(n.y);
		data.push_back(n.z);

		//uv
		const glm::vec2 &uvRef = uv[i];
		data.push_back(uvRef.x);
		data.push_back(uvRef.y);
	}

	// create buffers/arrays
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	glBindVertexArray(vao);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	float stride = (3 + 3 + 2) * sizeof(float);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));

	// vertex uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));

	glBindVertexArray(0);
}



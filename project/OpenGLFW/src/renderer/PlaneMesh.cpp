#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <io.h> // for C

#include "PlaneMesh.h"

using namespace std;

PlaneMesh::PlaneMesh()
{
	setupMesh(Type::pos);
}

PlaneMesh::PlaneMesh(std::shared_ptr<Texture> texture)
{
	textures.push_back(texture);
	setupMesh(Type::posNormalUV);
}

PlaneMesh::PlaneMesh(std::vector<std::shared_ptr<Texture>> textures)
	: textures(textures)
{
	setupMesh(Type::posNormalUV);
}

PlaneMesh::~PlaneMesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &_vbo);
}

void PlaneMesh::draw(Shader shader)
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
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void PlaneMesh::setupMesh(Type t /*= Type::pos*/)
{
	static const float s_planePos[] = {
		// positions          
		1.0f, -0.5f,  1.0f, 
		-1.0f, -0.5f,  1.0f, 
		-1.0f, -0.5f, -1.0f, 

		1.0f, -0.5f,  1.0f,  
		-1.0f, -0.5f, -1.0f, 
		1.0f, -0.5f, -1.0f, 
	};

	static const GLfloat s_planePosNormalUv[] = {
		// positions            // normals         // texcoords
		1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
		-1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

		1.0f, -0.5f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
		-1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		1.0f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
	};


	const float *planeVertices = nullptr;
	unsigned int planeMemSize = 0;

	switch (t)
	{
	default:
	case Type::pos:
			planeVertices = s_planePos;
			planeMemSize = sizeof(s_planePos);
			break;
	case Type::posNormal:
	case Type::posNormalUV:
		planeVertices = s_planePosNormalUv;
		planeMemSize = sizeof(s_planePosNormalUv);
		break;
	}

	// create buffers/arrays
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &_vbo);

	glBindVertexArray(vao);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, planeMemSize, planeVertices, GL_STATIC_DRAW);

	// set the vertex attribute pointers
	unsigned int floatCount = 3;
	if (t >= Type::posNormal) floatCount += 3;
	if (t >= Type::posNormalUV) floatCount += 2;

	unsigned int stride = floatCount * sizeof(GL_FLOAT);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	if (t >= Type::posNormal)
	{
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	}

	if (t >= Type::posNormalUV)
	{
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
	}

	glBindVertexArray(0);
}



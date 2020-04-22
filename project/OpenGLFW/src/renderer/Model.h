#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

class Model {
public:
	// stores all the textures loaded so far, optimization to make sure textures ares't loaded more than once.
	std::map<std::string, std::shared_ptr<Texture>> texturesCache;
	std::vector<std::shared_ptr<Mesh>> meshes;

private:
	std::string _directory;
	bool _isGammaCorrection;

public:
	Model(const std::string &path, bool isGammaCorrection) : _isGammaCorrection(isGammaCorrection) {
		loadModel(path);
	}

	void draw(Shader shader);

private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in meshes vector
	void loadModel(const std::string &path);
	void processNode(aiNode *node, const aiScene *scene);
	std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

};
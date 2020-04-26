#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <io.h> // for C

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Model.h"

using namespace std;

void Model::loadModel(const std::string & path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	//aiProcess_Triangulate we tell Assimp that if the model does not (entirely) consist of triangles it should transform all the model's primitive shapes to triangles
	// aiProcess_FlipUVs, most images in OpenGL were reversed around the y-axis so this little post-processing option fixes that for us
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "[E] assimp " << importer.GetErrorString() << endl;
	}

	_directory = path.substr(0, path.find_last_of('/'));
	//process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// after we've processed all of the meshes we then recursively process each of the children nodes.
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

const glm::vec3 &&convertToGLMVec3(const aiVector3D &input) {
	return glm::vec3(input.x, input.y, input.z);
}

const glm::vec2 &&convertToGLMVec2(const aiVector3D &input) {
	return glm::vec2(input.x, input.y);
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<shared_ptr<Texture>> textures;

	// 1.work through each of the mesh's vertices
	for (unsigned int i = 0; i< mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.pos = convertToGLMVec3(mesh->mVertices[i]);
		vertex.normal = convertToGLMVec3(mesh->mNormals[i]);
		if (mesh->mTextureCoords[0])
			// the mesh contain texture coordinates.
			vertex.texCoords = convertToGLMVec2(mesh->mTextureCoords[0][i]);
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);

		vertex.tangent = convertToGLMVec3(mesh->mTangents[i]);
		vertex.bitangent = convertToGLMVec3(mesh->mBitangents[i]);
		vertices.push_back(vertex);
	}

	// 2. work through each of the mesh's face(a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// 3.process materials
	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	/* we assume sampler names in shaders: each diffuse texture should be named as 'tex_diffuseN'
	 * where N is a sequential number ranging from 0 to MAX_SAMPLER_NUMBER - 1.
	 * like:
	 * diffuse: tex_diffuseN
	 * specular: tex_specularN
	 * normal: tex_normalN
	 */

	// 1. diffuse maps
	vector<shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "tex_diffuse");
	textures.insert(textures.end(), diffuseMaps.cbegin(), diffuseMaps.cend());

	// 2. specular maps
	vector<shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "tex_specular");
	textures.insert(textures.end(), specularMaps.cbegin(), specularMaps.cend());

	// 3. normal maps
	vector<shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "tex_normal");
	textures.insert(textures.end(), normalMaps.cbegin(), normalMaps.cend());

	// 4.height maps
	vector<shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "tex_height");
	textures.insert(textures.end(), heightMaps.cbegin(), heightMaps.cend());

	return std::make_shared<Mesh>(vertices, indices, textures);
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName)
{
	vector<shared_ptr<Texture>> textures;
	unsigned int textureCount = mat->GetTextureCount(type);

#if _LOG_ == 1
	if (textureCount > 0)
		cout << "-> loading " << typeName << ", count:" << textureCount << endl;
#endif

	for (unsigned int i = 0; i < textureCount; ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool isSkip = false;

		auto texIt = texturesCache.find(str.C_Str());
		if (texIt != texturesCache.end()) {
			// a texture with same filepath has already ben loaded, continue to next one(optimization)
			textures.push_back(texIt->second);
		}
		else {
			// if texture hasn't been loaded already, load it.
			string fullPath = _directory + "/" + replacePcxImageToPng(str.C_Str());
			auto tex = make_shared<Texture>(GL_TEXTURE_2D, fullPath);
			tex->setSamplerName(typeName, i);
			textures.push_back(tex);

			texturesCache.insert(std::make_pair(str.C_Str(), tex));
		}
	}

	return textures;
}

std::string Model::replacePcxImageToPng(const std::string & input)
{
	// replace pcx file to png file
	std::string newFilePath = input;
	size_t startPos = newFilePath.rfind(".pcx");
	if (startPos != std::string::npos)
	{
		newFilePath.replace(startPos, strlen(".pcx"), ".png");
	}

	return newFilePath;
}



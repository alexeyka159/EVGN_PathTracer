#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

class Model
{
private:
	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
	Shader* m_Shader;

	std::vector<Texture> m_TexturesLoaded;

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::TextureType typeName);

public:
	Model(const Model&) = default;
	inline Model(const char* path, Shader& shader) : m_Shader(&shader) { LoadModel(path); }
	void Draw();

	inline Shader& GetShader() { return *m_Shader; }
};
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model
{
private:
	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
	std::string m_Path;
	std::string m_Name = "";

	std::vector<Texture> m_TexturesLoaded;

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::TextureType typeName);

public:
	Model(const Model&) = default;
	inline Model(const char* path) : m_Path(path) { LoadModel(path); }
	void Draw(Shader &shader);

	inline std::string GetPath() { return m_Path; }
	inline std::string GetName() { return m_Name; }
};
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

struct Material
{
	std::vector<Texture> TexturesLoaded;

	glm::vec3 DiffuseColor = glm::vec3(0.9f);
	bool IsDiffuseUsing = false;
	float DiffuseValue = 0.f;
	float DiffuseContrast = 1.f;

	float MetallicColor = 0.f;
	bool IsMetallicUsing = false;

	glm::vec3 RoughnessColor = glm::vec3(0.5f);
	bool IsRoughnessUsing = false;
	bool IsRoughnessInvert = false;
	float RoughnessValue = 0.f;
	float RoughnessContrast = 1.f;

	bool IsNormalUsing = false;
	float NormalStrength = 1.f;
};

class Model
{
private:
	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
	std::string m_Path;
	std::string m_Name = "";

	//std::vector<Texture> m_TexturesLoaded;
	Material m_Materail;

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::TextureType typeName);

public:
	Model(const Model&) = default;
	inline Model(const char* path) : m_Path(path) { LoadModel(path); }
	void Draw(Shader &shader);

	inline Material& GetMaterial() { return m_Materail; }

	inline std::string GetPath() { return m_Path; }
	inline std::string GetName() { return m_Name; }
};
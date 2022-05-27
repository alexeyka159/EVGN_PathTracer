#include "Model.h"
#include <fstream>

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	m_Directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Name = mesh->mName.C_Str();
		m_Meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

static std::string GetPathFromMTL(std::string objPath, std::string mtlTexType)
{
	std::string mtlName = "";
	std::string texPath = "";

	{
		std::string directory;
		const size_t last_slash_idx = objPath.rfind('\\');
		if (std::string::npos != last_slash_idx)
		{
			directory = objPath.substr(0, last_slash_idx);
		}
		// get filename
		std::string base_filename = objPath.substr(objPath.find_last_of("/\\") + 1);

		// remove extension from filename
		std::string::size_type const p(base_filename.find_last_of('.'));
		std::string fileName = base_filename.substr(0, p);
		mtlName = directory + "\\" + fileName + ".mtl";

		mtlName = ReplaceAll(mtlName, "\\", "\\\\");
	}
	
	if (mtlName != "")
	{
		std::ifstream ifs(mtlName);

		while (std::getline(ifs, texPath))
		{
			size_t pos = texPath.find(mtlTexType);
			if (pos != std::string::npos)
			{
				texPath.erase(pos, mtlTexType.length() + 1);
				break;
			}
		}
		ifs.close();
	}
	return texPath;
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::Vertex vertex;
		// Обрабатываем координаты вершин, нормали и текстурные координаты
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps =
			LoadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::TextureType::DIFFUSE);
		if(diffuseMaps.size() > 0)
		{
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			m_Materail.IsDiffuseUsing = true;
		}

		std::vector<Texture> roughnessMaps =
			LoadMaterialTextures(material, aiTextureType_SHININESS, Texture::TextureType::ROUGHNESS);
		if (roughnessMaps.size() > 0)
		{
			textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
			m_Materail.IsRoughnessUsing = true;
		}
		
		std::string modelType = GetPath().substr(GetPath().find_last_of(".") + 1);
		if (modelType == "obj")
		{
			std::vector<Texture> normalMaps =
				LoadMaterialTextures(material, aiTextureType_HEIGHT, Texture::TextureType::NORMAL);
			if (normalMaps.size() > 0)
			{
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
				m_Materail.IsNormalUsing = true;
			}

			std::vector<Texture> metallicMaps;
			std::string metallicPath = GetPathFromMTL(m_Path, "refl");
			if (metallicPath != "")
			{
				metallicMaps.push_back(Texture(metallicPath, Texture::TextureType::METALLIC));
				m_Materail.TexturesLoaded.push_back(std::move(metallicMaps[metallicMaps.size() - 1]));
				m_Materail.IsMetallicUsing = true;
			}
		}
		else
		{
			std::vector<Texture> normalMaps =
				LoadMaterialTextures(material, aiTextureType_NORMALS, Texture::TextureType::NORMAL);
			if (normalMaps.size() > 0)
			{
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
				m_Materail.IsNormalUsing = true;
			}

			std::vector<Texture> metallicMaps =
				LoadMaterialTextures(material, aiTextureType_METALNESS, Texture::TextureType::METALLIC);
			if (metallicMaps.size() > 0)
			{
				textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
				m_Materail.IsMetallicUsing = true;
			}
		}
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::TextureType typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < m_Materail.TexturesLoaded.size(); j++)
		{
			if (std::strcmp(m_Materail.TexturesLoaded[j].GetPath().data(), str.C_Str()) == 0)
			{
				textures.push_back(m_Materail.TexturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			std::string filename = std::string(str.C_Str());
			filename = /*m_Directory + "/" + */filename;
			//Texture texture(filename, typeName);
			textures.push_back(Texture(filename, typeName));
			m_Materail.TexturesLoaded.push_back(std::move(textures[textures.size()-1]));
		}
	}

	return textures;
}

void Model::Draw(Shader& shader)
{
	for (Mesh& mesh : m_Meshes)
	{
		shader.SetUniform1f("u_material.ior",				m_Materail.IOR);

		shader.SetUniformVec3f("u_material.diffuseColor",	m_Materail.DiffuseColor);
		shader.SetUniform1i("u_material.isDiffuseUsing",	m_Materail.IsDiffuseUsing);
		shader.SetUniform1f("u_material.diffuseValue",		m_Materail.DiffuseValue);
		shader.SetUniform1f("u_material.diffuseContrast",	m_Materail.DiffuseContrast);

		shader.SetUniform1f("u_material.metallicColor",		m_Materail.MetallicColor);
		shader.SetUniform1i("u_material.isMetallicUsing",	m_Materail.IsMetallicUsing);

		shader.SetUniform1f("u_material.roughnessColor",    m_Materail.RoughnessColor.r);
		shader.SetUniform1i("u_material.isRoughnessUsing",	m_Materail.IsRoughnessUsing);
		shader.SetUniform1f("u_material.roughnessValue",	m_Materail.RoughnessValue);
		shader.SetUniform1f("u_material.isRoughnessInvert", m_Materail.IsRoughnessInvert);
		shader.SetUniform1f("u_material.roughnessContrast", m_Materail.RoughnessContrast);

		shader.SetUniform1i("u_material.isNormalUsing",		m_Materail.IsNormalUsing);
		shader.SetUniform1f("u_material.normalStrength",	m_Materail.NormalStrength);

		mesh.Draw(&shader);
	}
}

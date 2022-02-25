#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

#include "Shader.h"

class Mesh {
private:
	unsigned int m_VAO;

	unsigned int m_VBO;
	//VertexBufferLayout m_Layout;

	unsigned int m_IBO;

	void SetupMesh();
public:
	
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	/*struct TextureData
	{
		Texture Data;
		std::string Type;
		std::string Path;
	};*/

	std::vector<Vertex>       m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<Texture>  m_Textures;
	//Добавить класс материалов, в котором будут храниться текстуры и шейдер

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader* shader);
};
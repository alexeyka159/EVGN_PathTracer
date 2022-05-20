#include "Mesh.h"


Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
	: m_Vertices(vertices)
	, m_Indices(indices)
	, m_Textures(textures)
{
	SetupMesh();

    
}

void Mesh::SetupMesh()
{

    //std::cout << m_Vertices[0].Position.x << std::endl;
    //m_VBO = VertexBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));
	//m_VAO.Bind();
	//m_VBO.GenBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));
 //   m_IBO.GenBuffer(&m_Indices[0], m_Indices.size());

	//m_Layout.Push<float>(3); //�������
	//m_Layout.Push<float>(3); //�������
	//m_Layout.Push<float>(2); //���������� ����������
	//m_VAO.AddBuffer(m_VBO, m_Layout);

 //   //m_IBO.Bind();
	////m_IBO.SendData(&m_Indices[0], m_Indices.size());

	//m_VAO.Unbind();
	//m_VBO.Unbind();
	//m_IBO.Unbind();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);

    glBindVertexArray(m_VAO);

    // ��������� ������ � ��������� �����
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // ����� ������������� � ���������� ��, ��� ������������ � ������ �� ���������� ���������� �������� ����������������.
    // ����� ������� ����� � ���, ��� �� ����� ������ �������� ��������� �� ���������, � ��� ��������� ������������� � ������ ������ � ���������� ���� glm::vec3 (��� glm::vec2), ������� ����� ����� ������������ � ������ ������ ���� float, �� � � ����� � � �������� ������
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    // ������������� ��������� ��������� ���������

    // ���������� ������
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // ������� ������
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // ���������� ���������� ������
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // ������� ������
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader)
{
    unsigned int diffuseNr = 1;
    unsigned int roughnessNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        m_Textures[i].Bind(i);
        //glActiveTexture(GL_TEXTURE0 + i); // ����� ����������� ���������� ������ ���������� ����
        // �������� ����� �������� (�������� N � diffuse_textureN)
        std::string number;
        Texture::TextureType type = m_Textures[i].GetType();

        switch (type)
        {
            case Texture::TextureType::DIFFUSE:  number = std::to_string(diffuseNr++);  break;
            case Texture::TextureType::ROUGHNESS: number = std::to_string(roughnessNr++); break;
            case Texture::TextureType::NORMAL:   number = std::to_string(normalNr++);   break;
            case Texture::TextureType::HEIGHT:   number = std::to_string(heightNr++);   break;
        }

        shader->SetUniform1i(("u_material." + (TextureTypeConv::ConvertTypeToStr(type) + number)).c_str(), i);
        //glBindTexture(GL_TEXTURE_2D, m_Textures[i].m_RendererID);
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ��������� ������� ��������� ���������� �������� ���������� � �� �������������� ���������
    glActiveTexture(GL_TEXTURE0);
}

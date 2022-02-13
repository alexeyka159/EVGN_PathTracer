#include "Mesh.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
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

    glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader)
{
    unsigned int diffuseNr = 1;
    unsigned int roughnessNr = 1;
    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        m_Textures[i].Bind(i);
        // �������� ����� �������� (�������� N � diffuse_textureN)
        std::string number;
        std::string type = TextureTypeConv::ConvertTypeToStr(m_Textures[i].GetType());

        if (type == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (type == "texture_roughness")
            number = std::to_string(roughnessNr++);

        shader->SetUniform1i(("material." + type + number), i);
        m_Textures[i].Bind(i);
    }

    //!!��������. ����� ��� ����� ������������ ��������� � �������������� ���
    // ������������ ���

    
    //shader->Bind();
    /*m_VAO.Bind();

    glDrawElements(GL_TRIANGLES, m_IBO.GetCount(), GL_UNSIGNED_INT, nullptr);

    m_VAO.Unbind();
    glActiveTexture(GL_TEXTURE0);*/
    //shader->Unbind();

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ��������� ������� ��������� ���������� �������� ���������� � �� �������������� ���������
    glActiveTexture(GL_TEXTURE0);
}

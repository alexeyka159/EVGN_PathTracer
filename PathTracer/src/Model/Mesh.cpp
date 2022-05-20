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

	//m_Layout.Push<float>(3); //¬ершины
	//m_Layout.Push<float>(3); //Ќормали
	//m_Layout.Push<float>(2); //“екстурные координаты
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

    // «агружаем данные в вершинный буфер
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // —амое замечательное в структурах то, что расположение в пам€ти их внутренних переменных €вл€етс€ последовательным.
    // —мысл данного трюка в том, что мы можем просто передать указатель на структуру, и она прекрасно преобразуетс€ в массив данных с элементами типа glm::vec3 (или glm::vec2), который затем будет преобразован в массив данных типа float, ну а в конце Ц в байтовый массив
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    // ”станавливаем указатели вершинных атрибутов

    //  оординаты вершин
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Ќормали вершин
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // “екстурные координаты вершин
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Ќормали вершин
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
        //glActiveTexture(GL_TEXTURE0 + i); // перед св€зыванием активируем нужный текстурный юнит
        // ѕолучаем номер текстуры (значение N в diffuse_textureN)
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

    // —читаетс€ хорошей практикой возвращать значени€ переменных к их первоначальным значени€м
    glActiveTexture(GL_TEXTURE0);
}

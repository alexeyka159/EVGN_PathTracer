#include "EnvironmentMap.h"
#include "VertexBufferLayout.h"

#include <glm/gtc/matrix_transform.hpp>

EnvironmentMap::EnvironmentMap()
    : m_GenCubemapShader("datafiles/shaders/hdri/generateCubemap.vert", "datafiles/shaders/hdri/generateCubemap.frag")
    , m_BackgroundShader("datafiles/shaders/hdri/background.vert", "datafiles/shaders/hdri/background.frag")
    , m_IrradianceShader("datafiles/shaders/hdri/generateCubemap.vert", "datafiles/shaders/hdri/irradiance.frag")
    , m_PrefilterShader("datafiles/shaders/hdri/generateCubemap.vert", "datafiles/shaders/hdri/prefilter.frag")
    , m_BRDFShader("datafiles/shaders/hdri/brdf.vert", "datafiles/shaders/hdri/brdf.frag")
{
    Initialize(8);
}

EnvironmentMap::EnvironmentMap(const char* path, int outputSize, int prefilterSize)
    : m_Path(path)
    , m_PrefilterSize(prefilterSize)
    , m_OutputSize(outputSize)
    , m_GenCubemapShader("datafiles/shaders/hdri/generateCubemap.vert", "datafiles/shaders/hdri/generateCubemap.frag")
    , m_BackgroundShader("datafiles/shaders/hdri/background.vert", "datafiles/shaders/hdri/background.frag")
    , m_IrradianceShader("datafiles/shaders/hdri/generateCubemap.vert", "datafiles/shaders/hdri/irradiance.frag")
    , m_PrefilterShader("datafiles/shaders/hdri/generateCubemap.vert", "datafiles/shaders/hdri/prefilter.frag")
    , m_BRDFShader("datafiles/shaders/hdri/brdf.vert", "datafiles/shaders/hdri/brdf.frag")
{
    Initialize(outputSize);
    LoadEnvironmentMap(path);
}

void EnvironmentMap::Initialize(unsigned int size)
{
    float cubeVertices[108]{
       -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // передняя грань
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // грань слева
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

        // грань справа
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,

        // нижняя грань
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,

        // верхняя грань
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f , 1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f
    };

    VertexBuffer cubeVB(cubeVertices, 3 * 36 * sizeof(float));
    VertexBufferLayout cubeLayout;

    cubeLayout.Push<float>(3);
    m_CubeVA.AddBuffer(cubeVB, cubeLayout);

    m_CubeVA.Unbind();
    cubeVB.Unbind();

    
    float quadVertices[20]{
        // координаты      // текстурные координаты
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    VertexBuffer quadVB(quadVertices, 5 * 4 * sizeof(float));
    VertexBufferLayout quadLayout;

    quadLayout.Push<float>(3);
    quadLayout.Push<float>(2);
    m_QuadVA.AddBuffer(quadVB, quadLayout);

    m_QuadVA.Unbind();
    quadVB.Unbind();


    glGenFramebuffers(1, &m_CaptureFBO);
    glGenRenderbuffers(1, &m_CaptureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CaptureRBO);
}

void EnvironmentMap::LoadEnvironmentMap(const char* path, int outputSize, int prefilterSize)
{
    //Загрузка хдрки
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(path, &m_Width, &m_Height, &m_BPP, 0);

    if (data)
    {
        m_Path = path;
        m_OutputSize = outputSize;
        m_PrefilterSize = prefilterSize;

        glGenTextures(1, &m_HDRITextureID);
        glBindTexture(GL_TEXTURE_2D, m_HDRITextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        //Карта облученности и бэкграунд
        CreateCubemap(m_Properties.CubemapTexture, m_OutputSize, false);
        ConvertHDRIToCubemap();
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_Properties.CubemapTexture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        BakeIrradianceMap();

        //Карта отражений
        CreateCubemap(m_Properties.PrefilterMapTexture, m_PrefilterSize, true);
        Prefiltering();
        BakeBRDF();


        m_Properties.IsSet = true;

        std::cout << "    - Loaded enviroment map: \"" << path << "\"" << std::endl;
    }
    else
    {
        std::cout << "ERROR::ENVIROMENT MAP::FAILED TO LOAD HDR IMAGE: \"" << path << "\"" << std::endl;
    }
}

void EnvironmentMap::CreateCubemap(unsigned int& textureId, unsigned int size, bool createMipMap)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, createMipMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    if (createMipMap)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void EnvironmentMap::RenderToCubemap(Shader& shader, GLint sourceType, unsigned int& source, unsigned int& target, int& size)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(sourceType, source);

    glViewport(0, 0, size, size);
    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shader.Bind();
        shader.SetUniform1i("u_EnvironmentMap", 0);
        shader.SetUniformMat4f("u_Projection", captureProjection);
        shader.SetUniformMat4f("u_View", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, target, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube(shader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EnvironmentMap::RenderQuad()
{
    m_BRDFShader.Bind();
    m_QuadVA.Bind();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_QuadVA.Unbind();
    m_BRDFShader.Unbind();
}

void EnvironmentMap::ConvertHDRIToCubemap()
{
    RenderToCubemap(m_GenCubemapShader, GL_TEXTURE_2D, m_HDRITextureID, m_Properties.CubemapTexture, m_OutputSize);
}

void EnvironmentMap::BakeIrradianceMap()
{
    int irradianceMapSize = 32;
    CreateCubemap(m_Properties.IrradianceMapTexture, irradianceMapSize);

    RenderToCubemap(m_IrradianceShader, GL_TEXTURE_CUBE_MAP, m_Properties.CubemapTexture, m_Properties.IrradianceMapTexture, irradianceMapSize);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Properties.IrradianceMapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void EnvironmentMap::BakeBRDF()
{
    glGenTextures(1, &m_Properties.BRDFLUTTexture);

    // Выделяем необходимое количество памяти для LUT-текстуры
    glBindTexture(GL_TEXTURE_2D, m_Properties.BRDFLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Properties.BRDFLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EnvironmentMap::RenderCube(const Shader& shader)
{
    shader.Bind();
    m_CubeVA.Bind();
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    m_CubeVA.Unbind();
    shader.Unbind();
}

void EnvironmentMap::BindIBL(unsigned int irradianceSlot, unsigned int prefilterSlot, unsigned int brdfSlot)
{
    glActiveTexture(GL_TEXTURE0 + irradianceSlot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Properties.IrradianceMapTexture);

    glActiveTexture(GL_TEXTURE0 + prefilterSlot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Properties.PrefilterMapTexture);

    glActiveTexture(GL_TEXTURE0 + brdfSlot);
    glBindTexture(GL_TEXTURE_2D, m_Properties.BRDFLUTTexture);
}

void EnvironmentMap::Prefiltering()
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Properties.CubemapTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // Изменение размера фреймбуфера в соответствии с размерами мипмап-уровня
        unsigned int mipWidth = m_PrefilterSize * std::pow(0.5, mip);
        unsigned int mipHeight = m_PrefilterSize * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);

        m_PrefilterShader.Bind();
        m_PrefilterShader.SetUniform1i("u_EnvironmentMap", 0);
        m_PrefilterShader.SetUniformMat4f("u_Projection", captureProjection);
        m_PrefilterShader.SetUniform1f("u_Roughness", roughness);

        for (unsigned int i = 0; i < 6; ++i)
        {
            m_PrefilterShader.Bind();
            m_PrefilterShader.SetUniformMat4f("u_View", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_Properties.PrefilterMapTexture, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderCube(m_PrefilterShader);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void EnvironmentMap::Draw(glm::mat4 view, glm::mat4 projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Properties.CubemapTexture);
    m_BackgroundShader.Bind();
    m_BackgroundShader.SetUniformMat4f("u_View", view);
    m_BackgroundShader.SetUniformMat4f("u_Projection", projection);

    if (m_Properties.IsDrawingBackground && m_Properties.IsSet)
    {
        m_BackgroundShader.SetUniform1i("u_EnvironmentMap", 0);
        m_BackgroundShader.SetUniform1i("u_IsMapUsing", true);
        m_BackgroundShader.SetUniform1f("u_Intisity", m_Properties.Intensity);
        glm::mat4 rotation = glm::mat4(1);
        rotation = glm::rotate(rotation, glm::radians(m_Properties.Rotation), glm::vec3(0, 1, 0));
        m_BackgroundShader.SetUniformMat4f("u_Rotation", rotation);
    }
    else
    {
        m_BackgroundShader.SetUniform1i("u_IsMapUsing", false);
        m_BackgroundShader.SetUniformVec3f("u_EnvironmentColor", m_Properties.Color);
    }

    RenderCube(m_BackgroundShader);

   
}
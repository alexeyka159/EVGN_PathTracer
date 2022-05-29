#include "EnvironmentMap.h"
#include "VertexBufferLayout.h"

#include <glm/gtc/matrix_transform.hpp>

EnvironmentMap::EnvironmentMap(const char* path, int outputSize)
    : m_Path(path)
    , m_GenCubemapShader("res/shaders/hdri/generateCubemap.vert", "res/shaders/hdri/generateCubemap.frag")
    , m_BackgroundShader("res/shaders/hdri/background.vert", "res/shaders/hdri/background.frag")
    , m_IrradianceShader("res/shaders/hdri/generateCubemap.vert", "res/shaders/hdri/irradiance.frag")
    , m_OutputSize(outputSize)
{
    float cubeVertices[108] {
       -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // передн€€ грань
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

        // нижн€€ грань
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,

        // верхн€€ грань
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
    m_VA.AddBuffer(cubeVB, cubeLayout);

    m_VA.Unbind();
    cubeVB.Unbind();

    glGenFramebuffers(1, &m_CaptureFBO);
    glGenRenderbuffers(1, &m_CaptureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_OutputSize, m_OutputSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CaptureRBO);

    LoadEnvironmentMap(path);
}

void EnvironmentMap::LoadEnvironmentMap(const char* path, int outputSize)
{
    //«агрузка хдрки
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(path, &m_Width, &m_Height, &m_BPP, 0);

    if (data)
    {
        m_Path = path;
        m_OutputSize = outputSize;

        glGenTextures(1, &m_HDRITextureID);
        glBindTexture(GL_TEXTURE_2D, m_HDRITextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        CreateCubemap(m_RendererID, m_OutputSize);

        ConvertHDRIToCubemap();
        BakeIrradianceMap();

        std::cout << "    - Loaded enviroment map: \"" << path << "\"" << std::endl;
    }
    else
    {
        std::cout << "ERROR::ENVIROMENT MAP::FAILED TO LOAD HDR IMAGE: \"" << path << "\"" << std::endl;
    }
}

void EnvironmentMap::CreateCubemap(unsigned int& textureId, unsigned int size)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }
}

void EnvironmentMap::RenderToCubemap(Shader& shader, GLint sourceType, unsigned int& source, unsigned int& target, int& size)
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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

        RenderCube(m_VA, shader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void EnvironmentMap::ConvertHDRIToCubemap()
{
    RenderToCubemap(m_GenCubemapShader, GL_TEXTURE_2D, m_HDRITextureID, m_RendererID, m_OutputSize);
}

void EnvironmentMap::BakeIrradianceMap()
{
    int irradianceMapSize = 32;
    CreateCubemap(m_IrradianceMap, irradianceMapSize);

    RenderToCubemap(m_IrradianceShader, GL_TEXTURE_CUBE_MAP, m_RendererID, m_IrradianceMap, irradianceMapSize);
}

void EnvironmentMap::RenderCube(const VertexArray& va, const Shader& shader)
{
    shader.Bind();
    va.Bind();
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    va.Unbind();
    shader.Unbind();
}

void EnvironmentMap::BindIrradianceMap(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
}

void EnvironmentMap::Draw(glm::mat4 view, glm::mat4 projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
    m_BackgroundShader.Bind();
    m_BackgroundShader.SetUniformMat4f("u_View", view);
    m_BackgroundShader.SetUniformMat4f("u_Projection", projection);
    m_BackgroundShader.SetUniform1i("u_EnvironmentMap", 0);

    RenderCube(m_VA, m_BackgroundShader);
}
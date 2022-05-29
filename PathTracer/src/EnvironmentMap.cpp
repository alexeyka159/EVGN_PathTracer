#include "EnvironmentMap.h"
#include "VertexBufferLayout.h"

#include <glm/gtc/matrix_transform.hpp>

EnvironmentMap::EnvironmentMap(const char* path, int outputSize)
    : m_Path(path)
    , m_GenCubemapShader("res/shaders/hdri/generateCubemap.vert", "res/shaders/hdri/generateCubemap.frag")
    , m_BackgroundShader("res/shaders/hdri/background.vert", "res/shaders/hdri/background.frag")
    , m_OutputSize(outputSize)
{
    float m_CubeVertices[108] {
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

    VertexBuffer cubeVB(m_CubeVertices, 3 * 36 * sizeof(float));
    VertexBufferLayout cubeLayout;

    cubeLayout.Push<float>(3);
    m_VA.AddBuffer(cubeVB, cubeLayout);

    m_VA.Unbind();
    cubeVB.Unbind();

    LoadEnvironmentMap(path);
}

void EnvironmentMap::LoadEnvironmentMap(const char* path, int outputSize)
{
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


        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_OutputSize, m_OutputSize, 0, GL_RGB, GL_FLOAT, nullptr);
        }

        BakeToFramebuffer();

        std::cout << "    - Loaded enviroment map: \"" << path << "\"" << std::endl;
    }
    else
    {
        std::cout << "ERROR::ENVIROMENT MAP::FAILED TO LOAD HDR IMAGE: \"" << path << "\"" << std::endl;
    }
}

void EnvironmentMap::BindTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void EnvironmentMap::BakeToFramebuffer()
{
    glGenFramebuffers(1, &m_CaptureFBO);
    glGenRenderbuffers(1, &m_CaptureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_CaptureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_OutputSize, m_OutputSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_CaptureRBO);


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

    // Конвертирование равнопромежуточной HDR-карты окружения в кубическую карту

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_HDRITextureID);

    
    glViewport(0, 0, m_OutputSize, m_OutputSize);
    glBindFramebuffer(GL_FRAMEBUFFER, m_CaptureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        m_GenCubemapShader.Bind();
        m_GenCubemapShader.SetUniform1i("u_EquirectangularMap", 0);
        m_GenCubemapShader.SetUniformMat4f("u_Projection", captureProjection);
        m_GenCubemapShader.SetUniformMat4f("u_View", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_RendererID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderCube(m_VA, m_GenCubemapShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void EnvironmentMap::Draw(glm::mat4 view, glm::mat4 projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
    m_BackgroundShader.Bind();
    m_BackgroundShader.SetUniformMat4f("u_View", view);
    m_BackgroundShader.SetUniformMat4f("u_Projection", projection);
    m_BackgroundShader.SetUniform1i("u_EnvironmentMap", 0);

    RenderCube(m_VA, m_BackgroundShader);



    /*glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_HDRITextureID);

    m_GenCubemapShader.Bind();
    m_GenCubemapShader.SetUniform1i("u_EquirectangularMap", 0);
    m_GenCubemapShader.SetUniformMat4f("u_Projection", projection);
    m_BackgroundShader.SetUniformMat4f("u_View", view);
    RenderCube(m_VA, m_GenCubemapShader);*/

}
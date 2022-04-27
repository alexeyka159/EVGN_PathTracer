#include "ViewportController.h"

ViewportController::ViewportController(Framebuffer& framebuffer, ImVec2* viewportOffset, ImVec2* windowSize, ImVec2* minBound, ImVec2* mousePos)
    : m_Framebuffer(&framebuffer)
    , m_ViewportOffset(viewportOffset)
    , m_WindowSize(windowSize)
    , m_MinBound(minBound)
    , m_MousePos(mousePos)
{
}

void ViewportController::InputProcessor(GLFWwindow* window)
{
	
}

void ViewportController::MouseProcessor(GLFWwindow* window)
{
    float mx = m_MousePos->x;
    float my = m_MousePos->y;
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y - my - 20;

    int mouseX = (int)mx;
    int mouseY = (int)my;

    std::cout << "Mouse = " << mouseX << " " << mouseY << std::endl;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        m_Framebuffer->Bind();

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            //std::cout << "Mouse = " << mouseX << ", " << mouseY << std::endl;
            std::cout << "Entity ID = " << pixelData << std::endl;
        }

        //auto viewportOffset = ImGui::GetCursorPos();

        //auto windowSize = ImGui::GetWindowSize();
        //ImVec2 minBound = ImGui::GetWindowPos();
        m_MinBound->x += m_ViewportOffset->x;
        m_MinBound->y += m_ViewportOffset->y;

        ImVec2 maxBound = { m_MinBound->x + m_WindowSize->x, m_MinBound->y + m_WindowSize->y };
        m_ViewportBounds[0] = { m_MinBound->x, m_MinBound->y };
        m_ViewportBounds[1] = { maxBound.x, maxBound.y };

        //m_Framebuffer->Unbind();
    }
}

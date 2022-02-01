#include "EVGNTime.h"
#include <GLFW/glfw3.h>

EVGN::Time::Time()
{
}

void EVGN::Time::UpdateTime()
{
	float currentFrame = glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;
}

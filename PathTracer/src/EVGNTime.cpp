#include "EVGNTime.h"
#include <GLFW/glfw3.h>

EVGN::Time::Time()
{
}

void EVGN::Time::UpdateTime()
{
	m_totalTime = glfwGetTime();
	m_deltaTime = m_totalTime - m_lastFrame;
	m_lastFrame = m_totalTime;
}

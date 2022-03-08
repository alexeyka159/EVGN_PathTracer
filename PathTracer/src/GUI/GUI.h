#pragma once

#include <GLFW/glfw3.h>

#include <vector>

class GUIPanel;
class GUI
{
private:
	GLFWwindow* m_Window;

	std::vector<GUIPanel*> m_Panels;

public:
	GUI(GLFWwindow* window);
	~GUI();

	inline void Push(GUIPanel* panel) { m_Panels.emplace_back(panel); };

	void Begin();
	void End();
	void Render();
};
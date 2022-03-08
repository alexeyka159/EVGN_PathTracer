#pragma once

#include <GLFW/glfw3.h>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "GUI/Panels/ViewportPanel.h"

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
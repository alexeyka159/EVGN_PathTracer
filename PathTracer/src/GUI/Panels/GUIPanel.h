#pragma once

#include <string>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

class GUIPanel
{
private:
	std::string m_Tag = std::string("");
public:
	virtual void Draw() = 0;
	inline ~GUIPanel() { delete this; };
};
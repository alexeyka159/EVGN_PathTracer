#pragma once

#include <iostream>

#include <glm/glm.hpp>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "InputController.h"
#include "FrameBuffer.h"

class ViewportController : public InputController
{
private:
	glm::vec2 m_ViewportBounds[2];
	Framebuffer* m_Framebuffer;

	ImVec2* m_ViewportOffset;
	ImVec2* m_WindowSize;
	ImVec2* m_MinBound;
	ImVec2* m_MousePos;

public:
	ViewportController(Framebuffer& framebuffer, ImVec2* viewportOffset, ImVec2* windowSize, ImVec2* minBound, ImVec2* mousePos);
	inline ~ViewportController() = default;

	void InputProcessor(GLFWwindow* window) override;
	void MouseProcessor(GLFWwindow* window) override;
};
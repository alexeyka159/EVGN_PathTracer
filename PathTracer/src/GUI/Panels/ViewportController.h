#pragma once

#include "InputController.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera/Camera.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
class ViewportController : public InputController
{
private:
	bool m_MouseState = false;

	double m_EndPosX = 475, m_EndPosY = 270;
	double m_StartPosX = m_EndPosX, m_StartPosY = m_EndPosY;
	glm::vec3 m_SpawnPos{ 0 };
	glm::vec3 m_ReleasePos{ 0 };
	bool m_Editable = false;

	Camera* m_Camera;
	Scene* m_Context;
	Model* m_DefaultModel;
	Shader* m_TrailShader;

	glm::vec2 m_ScreenSize { 0 };
	
	void CreateBody();
	glm::vec3 ScreenToWorldSpace(double& x, double& y);
public:
	ViewportController(Scene& context, Camera& camera, Model& defaultModel, Shader& trailShader);
	inline ~ViewportController() = default;

	inline void SetScreenSize(float& width, float& height) { m_ScreenSize = glm::vec2(width, height); };
	inline void SetEditable(bool isEditable) { m_Editable = isEditable; }
	void InputProcessor(GLFWwindow* window) override;
	void MouseProcessor(GLFWwindow* window) override;
};
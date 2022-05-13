#pragma once

#include "glm/glm.hpp"
#include "Model/Model.h"
#include <glm/gtx/quaternion.hpp>
#include "Camera/CameraOrbit.h"

struct TagComponent
{
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag)
		: Tag(tag) {}
};

struct TransformComponent
{
	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::vec3 & translation)
		: Translation(translation) {}

	glm::mat4 GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}
};

struct ModelRendererComponent
{
	Model ModelObj;

	ModelRendererComponent() = default;
	ModelRendererComponent(const ModelRendererComponent&) = default;
	ModelRendererComponent(const Model& model)
		: ModelObj(model) {}
	ModelRendererComponent(const char* path)
		: ModelObj(path) {}

	operator Model& () { return ModelObj; }
	operator const Model& () const { return ModelObj; }
};

struct CameraComponent
{
	CameraOrbit* RenderCamera = nullptr;
	bool Primary; //Текущая камера в редакторе
	//bool Render; //Камера для финального рендера

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(CameraOrbit& camera, bool isPrimary = false)
		: RenderCamera(&camera)
		, Primary(isPrimary) {}
	CameraComponent(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 upVec, float fov, float near, float far, float camSpeed = 5.5f, bool isPrimary = false)
		: Primary(isPrimary)
	{
		RenderCamera = new CameraOrbit(camPos, camTarget, upVec, fov, near, far, camSpeed);
	}
};

struct PointLightComponent
{
	glm::vec3 Color = { 1, 1, 1 };
	float Intensity = 1.f;

	float Constant = 1.f;
	float Linear = 0.35f;
	float Quadratic = 0.44f;

	PointLightComponent() = default;
	PointLightComponent(const PointLightComponent&) = default;
	PointLightComponent(glm::vec3& color, float& intensity, float constant, float linear, float quadratic)
		: Color(color)
		, Intensity(intensity)
		, Constant(constant)
		, Linear(linear)
		, Quadratic(quadratic) {}
	PointLightComponent(glm::vec3& color, float& intensity)
		: Color(color)
		, Intensity(intensity) {}
};

struct DirectionalLightComponent
{
	glm::vec3 Color = { 1, 1, 1 };
	float Intensity = 1.f;


	DirectionalLightComponent() = default;
	DirectionalLightComponent(const DirectionalLightComponent&) = default;
	DirectionalLightComponent(glm::vec3& color, float& intensity)
		: Color(color)
		, Intensity(intensity) {}
};

struct SpotLightComponent
{
	glm::vec3 Color = { 1, 1, 1 };
	float Intensity = 1.f;
	float CutOff = -0.7f;
	float OuterCutOff = -0.6f;

	float Constant = 1.f;
	float Linear = 0.35f;
	float Quadratic = 0.44f;

	SpotLightComponent() = default;
	SpotLightComponent(const SpotLightComponent&) = default;
	SpotLightComponent(glm::vec3& color, float& intensity, float cutOff, float outerCutOff, float constant, float linear, float quadratic)
		: Color(color)
		, Intensity(intensity)
		, CutOff(cutOff)
		, OuterCutOff(outerCutOff)
		, Constant(constant)
		, Linear(linear)
		, Quadratic(quadratic) {}
	SpotLightComponent(glm::vec3& color, float& intensity, float cutOff, float outerCutOff)
		: Color(color)
		, Intensity(intensity)
		, CutOff(cutOff)
		, OuterCutOff(outerCutOff) {}
};
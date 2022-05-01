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
};
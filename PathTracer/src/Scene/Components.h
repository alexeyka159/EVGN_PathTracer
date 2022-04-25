#pragma once

#include "glm/glm.hpp"
#include "Model/Model.h"

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
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
			* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });
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
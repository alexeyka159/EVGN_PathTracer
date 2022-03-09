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
	glm::mat4 Transform = glm::mat4(1.0f);

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::mat4& transform)
		: Transform(transform) {}

	operator glm::mat4& () { return Transform; }
	operator const glm::mat4& () const { return Transform; }
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
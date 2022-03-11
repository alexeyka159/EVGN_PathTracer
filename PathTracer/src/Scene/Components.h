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
	ModelRendererComponent(const char* path, Shader& shader)
		: ModelObj(path, shader) {}

	operator Model& () { return ModelObj; }
	operator const Model& () const { return ModelObj; }
};

struct GravityComponent
{
	float Mass = 1;
	float Radius = 1;
	glm::vec3 InitialVelocity { 1 };
	glm::vec3 CurrentVelocity { 0 };

	GravityComponent() { CurrentVelocity = InitialVelocity; };
	GravityComponent(const GravityComponent&) = default;
	GravityComponent(const float& mass, const float& radius, const glm::vec3& initialVelocity)
		: Mass(mass)
		, Radius(radius)
		, InitialVelocity(initialVelocity)
	{
		CurrentVelocity = InitialVelocity; 
	}
};
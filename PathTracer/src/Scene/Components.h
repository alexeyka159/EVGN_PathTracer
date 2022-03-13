#pragma once

#include "glm/glm.hpp"
#include "Model/Model.h"
#include "Shader.h"
#include <vector>

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

//TODO: Добавить свойство bool rendereble, чтобы была возможность отключать отображение модели во вьюпорте
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
	glm::vec3 InitialVelocity { 0 };
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


struct TrailComponent
{
	std::vector<glm::vec3> Positions;
	Shader* TrailShader;
	float Timer = 0;
	glm::vec3 Color;

	//TrailComponent() = default;
	TrailComponent(const TrailComponent&) = default;
	/*TrailComponent(const std::vector<glm::vec3>& coords, Shader shader)
		: Positions(coords)
		, TrailShader(shader){};*/
	TrailComponent(Shader* shader, glm::vec3& color)
		: TrailShader(shader)
		, Color(color) {}
	TrailComponent(Shader* shader)
		: TrailShader(shader)
	{
		float r = .5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1-.5f)));
		float g = .5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1-.5f)));
		float b = .5f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1-.5f)));
		Color = { r, g, b };
	};
};

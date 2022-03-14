#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
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
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f};

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::vec3& translation)
		: Translation(translation) {}

	glm::mat4 GetTransform() const
	{
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, {1, 0, 0})
			* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
			* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });
		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}
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

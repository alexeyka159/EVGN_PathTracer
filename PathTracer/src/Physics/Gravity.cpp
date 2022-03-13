#include "Gravity.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

void Gravity::Update(Scene& scene, const float timeStep, bool rememberTrails)
{
	auto group = scene.m_Registry.group<GravityComponent>(entt::get<TransformComponent>);

	for(auto entity : group) {
		for (auto otherEntity : group)
		{
			if (entity != otherEntity)
			{
				const auto [transform, body]		   = group.get<TransformComponent, GravityComponent>(entity);
				const auto [otherTransform, otherBody] = group.get<TransformComponent, GravityComponent>(otherEntity);
				
				glm::vec3 position		= transform.Transform * glm::vec4(1);
				glm::vec3 otherPosition = otherTransform.Transform * glm::vec4(1);
				
				float sqrDst = glm::length(otherPosition - position);
				sqrDst *= sqrDst;

				glm::vec3 forceDir = glm::normalize(otherPosition - position);
				glm::vec3 force = forceDir * m_GravitationalConstant * body.Mass * otherBody.Mass / sqrDst;
				glm::vec3 acceleration = force / body.Mass;
				body.CurrentVelocity += acceleration * timeStep * m_SimulationSpeed;
			}
		}
	}

	for (auto entity : group) {
		const auto& [transform, body] = group.get<TransformComponent, GravityComponent>(entity);
		auto& trail = scene.m_Registry.get<TrailComponent>(entity);

		transform.Transform = glm::translate(transform.Transform, body.CurrentVelocity * timeStep);

		//std::cout << trail.Timer << std::endl;
		if (rememberTrails && trail.Timer > 0.08f)
		{
			if (trail.Positions.size() > 256*2)
				trail.Positions.erase(trail.Positions.begin());
			trail.Positions.push_back(transform.Transform * glm::vec4(0.f, 0.f, 0.f, 1.0f));
			trail.Timer = 0;
		}
		trail.Timer += timeStep;
	}

}

#pragma once

#include <vector>
#include "glm/glm.hpp"

class Scene;
class Gravity
{
private:
	const float m_GravitationalConstant = .9f;
	std::vector<glm::vec3> m_CelestialBodyPaths;
	float m_SimulationSpeed = 1;

public:
	Gravity() = default;

	void Update(Scene& scene, float timeStep, bool rememberTrails = true);
	inline std::vector<glm::vec3>& GetTrails() { return m_CelestialBodyPaths; };
	inline float& GetSimulationSpeed() { return m_SimulationSpeed; }
};
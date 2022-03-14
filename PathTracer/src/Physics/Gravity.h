#pragma once

#include <vector>
#include "glm/glm.hpp"

class Scene;
class Gravity
{
private:
	const float m_GravitationalConstant = .9f;
	std::vector<glm::vec3> m_CelestialBodyPaths;

public:
	float simulationSpeed = 1;
	Gravity() = default;

	void Update(Scene& scene, float timeStep, bool rememberTrails = true);
	inline std::vector<glm::vec3>& GetTrails() { return m_CelestialBodyPaths; };
};
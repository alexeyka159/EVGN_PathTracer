#pragma once

#include <vector>

class Scene;
class Gravity
{
private:
	const float m_GravitationalConstant = .9f;

public:
	Gravity() = default;

	void Update(Scene& scene, float timeStep);
};
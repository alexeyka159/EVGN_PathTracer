#pragma once

namespace EVGN {
	class Time {
	private:
		float m_deltaTime = 0.0f;	// время между текущим и последним кадрами
		float m_lastFrame = 0.0f; // время последнего кадра

	public:

		Time();
		void UpdateTime();
		inline float DeltaTime() { return m_deltaTime; };
	};
}
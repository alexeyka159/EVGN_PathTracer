#pragma once

namespace EVGN {
	class Time {
	private:
		float m_deltaTime = 0.0f;	// ����� ����� ������� � ��������� �������
		float m_lastFrame = 0.0f; // ����� ���������� �����

	public:

		Time();
		void UpdateTime();
		inline float DeltaTime() { return m_deltaTime; };
	};
}
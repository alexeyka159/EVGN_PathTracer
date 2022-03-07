#pragma once

#include <string>

class GUIPanel
{
private:
	std::string m_Tag = std::string("");
public:
	virtual void Draw() = 0;
	inline ~GUIPanel() { delete this; };
};
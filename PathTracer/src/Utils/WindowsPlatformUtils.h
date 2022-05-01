#pragma once

#include <string>

struct GLFWwindow;

class FileDialogs
{
public:
	static std::string OpenFile(const char* filter, GLFWwindow* window);
	static std::string SaveFile(const char* filter, GLFWwindow* window);
};
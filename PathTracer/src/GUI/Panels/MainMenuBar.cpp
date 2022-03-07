#include "MainMenuBar.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

void MainMenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl N"))
			{
				//Do something
			}
			if (ImGui::MenuItem("Open", "Ctrl O"))
			{
				//Do something
			}
			if (ImGui::MenuItem("Save", "Ctrl S"))
			{
				//Do something
			}
			if (ImGui::MenuItem("Save As", "Ctrl Shift S"))
			{
				//Do something
			}
			if (ImGui::MenuItem("Import"))
			{
				//Do something
			}
			if (ImGui::MenuItem("Export"))
			{
				//Do something
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Edit"))
		{
		}
		if (ImGui::MenuItem("Add"))
		{
		}
		if (ImGui::MenuItem("Selection"))
		{
		}
		ImGui::EndMainMenuBar();
	}
}

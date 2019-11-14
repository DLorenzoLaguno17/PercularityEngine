#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "imgui.h"

#include "OpenGL.h"
#include "DevIL/include/ilut.h":

ComponentMaterial::ComponentMaterial(COMPONENT_TYPE type, GameObject* parent, bool active) : 
	Component(type, parent, active) {}

void ComponentMaterial::Update() {

}

void ComponentMaterial::OnEditor() {

	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Checkbox(tex_name.c_str(), &active);
		ImGui::Text("Size: %dx%d", height, width);
		ImGui::NewLine();

		ImGui::Image((void*)texture, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::NewLine();
	}
}
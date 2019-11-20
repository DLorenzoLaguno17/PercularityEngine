#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ResourceTexture.h"
#include "GameObject.h"
#include "ImGui/imgui.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, bool active) : 
	Component(COMPONENT_TYPE::MATERIAL, parent, active) {}

void ComponentMaterial::OnEditor() {

	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Checkbox(resource_tex->name.c_str(), &active);
		ImGui::Text("Size: %dx%d", resource_tex->height, resource_tex->width);
		ImGui::NewLine();

		ImGui::Image((void*)resource_tex->texture, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::NewLine();
	}
}

// Load & Save 
void ComponentMaterial::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file) {
	
	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Active"];
}

void ComponentMaterial::OnSave(const char* gameObjectNum, nlohmann::json &scene_file) {
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Active"] = active;
}
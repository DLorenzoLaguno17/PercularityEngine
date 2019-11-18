#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "GameObject.h"
#include "ImGui/imgui.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, bool active) : 
	Component(COMPONENT_TYPE::MATERIAL, parent, active) {}

void ComponentMaterial::OnEditor() {

	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Checkbox(tex_name.c_str(), &active);
		ImGui::Text("Size: %dx%d", height, width);
		ImGui::NewLine();

		ImGui::Image((void*)texture, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::NewLine();
	}
}

// Load & Save 
void ComponentMaterial::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file) {
	json js = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Name"];
	tex_name = js.get<std::string>();

	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Active"];

	texture = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Texture"];
	width = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Width"];
	height = scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Height"];
}

void ComponentMaterial::OnSave(const char* gameObjectNum, nlohmann::json &scene_file) {
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Active"] = active;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Name"] = tex_name;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Texture"] = texture;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Width"] = width;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Material"]["Height"] = height;
}
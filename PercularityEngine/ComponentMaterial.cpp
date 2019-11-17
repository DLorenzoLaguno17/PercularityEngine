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
void ComponentMaterial::OnLoad(const char* scene_name, const nlohmann::json &scene_file) {

	UUID = scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["UUID"];
	parent_UUID = scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Parent UUID"];
	active = scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Active"];
	//tex_name = scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Name"];
	texture = scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Texture"];
	width = scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Width"];
	height = scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Height"];
}

void ComponentMaterial::OnSave(const char* scene_name, nlohmann::json &scene_file) {
	scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["UUID"] = UUID;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Parent UUID"] = parent_UUID;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Active"] = active;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Name"] = tex_name;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Texture"] = texture;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Width"] = width;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Components"]["Material"]["Height"] = height;
}
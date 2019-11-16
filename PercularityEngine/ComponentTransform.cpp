#include "ComponentTransform.h"
#include "imgui.h"
#include "GameObject.h"
#include "Application.h"

ComponentTransform::ComponentTransform(GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::TRANSFORM, parent, active)
{
	UUID = (uint)App->GetRandomGenerator().Int();
	if (parent) parent_UUID = parent->GetUUID();
}

void ComponentTransform::OnEditor() {

	if (ImGui::CollapsingHeader("Transform")) {

		ImGui::DragFloat3("Translation", &translation.x,0.05f);
		ImGui::DragFloat3("Scale", &scale.x, 0.05f);

		uiRotation = rotation.ToEulerXYZ();
		uiRotation *= 180 / pi;
		ImGui::DragFloat3("Rotation", &uiRotation.x, 0.5f);
		uiRotation /= 180 / pi;
		rotation = Quat::FromEulerXYZ(uiRotation.x, uiRotation.y, uiRotation.z);

		// We check if it must be updated
		if (!lastRotation.Equals(rotation) || !lastTranslation.Equals(translation) || !lastScale.Equals(scale)) {
			UpdateTransform();

			lastRotation = rotation;
			lastTranslation = translation;
			lastScale = scale;
		}

		ImGui::NewLine();
	}
}

void ComponentTransform::UpdateTransform()
{
	localTransform = float4x4::FromTRS(translation, rotation, scale);
	
	if (parent != nullptr)
		globalTransform = parent->transform->globalTransform * localTransform;

	UpdateRenderTransform();

	for (int i = 0; i < parent->children.size(); ++i) {
		parent->children[i]->transform->UpdateTransform();
	}	
}

void ComponentTransform::UpdateRenderTransform()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j)
			renderTransform.M[i * 4 + j] = localTransform[j][i];
	}
}

void ComponentTransform::SetPosition(float3 position)
{
	translation = position;
	UpdateTransform();
}

void ComponentTransform::Move(float3 positionIncrease)
{
	translation += positionIncrease;
	UpdateTransform();
}

void ComponentTransform::Scale(float3 newScale)
{
	scale += newScale;
	UpdateTransform();
}

// Load & Save 
void ComponentTransform::OnLoad(const char* scene_name, const nlohmann::json &scene_file) {
	UUID = scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["UUID"];
	parent_UUID = scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Parent UUID"];
	/*rotation = scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Rotation"];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Rotation"];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Rotation"][rotation.z];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Translation"][scale.x];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Translation"][scale.y];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Translation"][scale.z];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Scale"][scale.x];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Scale"][scale.y];
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Scale"][scale.z];*/

}

void ComponentTransform::OnSave(const char* scene_name, nlohmann::json &scene_file) {
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["UUID"] = UUID;
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Parent UUID"] = parent_UUID;
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Rotation"] = {rotation.x, rotation.y, rotation.z, rotation.w };
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Translation"] = { translation.x, translation.y, translation.z };
	scene_file[scene_name]["Game Objects"]["Components"]["Transform"]["Scale"] = { scale.x, scale.y, scale.y };
}
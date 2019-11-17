#include "ComponentTransform.h"
#include "Application.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "ImGui/ImGuizmo.h"

ComponentTransform::ComponentTransform(GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::TRANSFORM, parent, active)
{
	UUID = (uint)App->GetRandomGenerator().Int();
	if (parent) parent_UUID = parent->GetUUID();
}

void ComponentTransform::Update()
{
	UpdateTransform();
}

void ComponentTransform::OnEditor() {

	if (ImGui::CollapsingHeader("Transform")) {

		float4x4 model = gameObject->transform->GetLocalTransform();
		model.Transpose();

		float3 translationM = GetTranslation();;
		float3 rotationM = GetEulerRotation();
		float3 scaleM = GetScale();

		if (ImGui::Button("Reset transform"))
			SetToZero();

		if (ImGui::DragFloat3("Translation", (float*)&translationM, 0.3))
			SetPosition(translationM);

		if (ImGui::DragFloat3("Rotation", (float*)&rotationM, 0.3))
			SetEulerRotation(rotationM);

		if (ImGui::DragFloat3("Scale", (float*)&scaleM, 0.3))
			SetScale(scaleM);
	}
}

void ComponentTransform::UpdateTransform()
{
	if (mustUpdate)
	{
		localTransform = float4x4::FromTRS(translation, rotation, scale);

		if (strcmp("World", gameObject->name.c_str()) != 0) 
			globalTransform = gameObject->parent->transform->GetGlobalTransform() * localTransform;

		UpdateRenderTransform();
		mustUpdate = false;

		gameObject->UpdateAABB();
	}
}

void ComponentTransform::UpdateRenderTransform()
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			renderTransform.M[i * 4 + j] = globalTransform[j][i];
}

void ComponentTransform::SetPosition(float3 newPosition)
{
	translation = newPosition;
	mustUpdate = true;
}

void ComponentTransform::Move(float3 positionIncrease)
{
	translation += positionIncrease;
	mustUpdate = true;
}

void ComponentTransform::Scale(float3 scale_)
{
	scale += scale_;
	mustUpdate = true;
}

void ComponentTransform::SetLocalTransform(float4x4 transform)
{
	transform.Decompose(translation, rotation, scale);
	eulerRotation = rotation.ToEulerXYZ().Abs();
	mustUpdate = true;
}

void ComponentTransform::SetEulerRotation(float3 eulerAngle)
{
	float3 angleIncrease = (eulerAngle - eulerRotation)*DEGTORAD;
	Quat newRotation = Quat::FromEulerXYZ(angleIncrease.x, angleIncrease.y, angleIncrease.z);
	rotation = rotation * newRotation;
	eulerRotation = eulerAngle;
	mustUpdate = true;
}

void ComponentTransform::SetScale(float3 newScale)
{
	scale = newScale;
	mustUpdate = true;
}

void ComponentTransform::SetToZero()
{
	scale = { 1,1,1 };
	translation = { 0,0,0 };
	rotation = Quat::identity;
	UpdateEulerRotation();

	mustUpdate = true;
}

void ComponentTransform::UpdateEulerRotation()
{
	eulerRotation = rotation.ToEulerXYZ()*RADTODEG;
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
	scene_file[scene_name]["Game Objects"][gameObject->name]["UUID"] = UUID;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Parent UUID"] = parent_UUID;
	scene_file[scene_name]["Game Objects"][gameObject->name]["Rotation"] = {rotation.x, rotation.y, rotation.z, rotation.w };
	scene_file[scene_name]["Game Objects"][gameObject->name]["Translation"] = { translation.x, translation.y, translation.z };
	scene_file[scene_name]["Game Objects"][gameObject->name]["Scale"] = { scale.x, scale.y, scale.y };
}
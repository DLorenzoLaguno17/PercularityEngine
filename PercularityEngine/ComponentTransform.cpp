#include "ComponentTransform.h"
#include "imgui.h"
#include "GameObject.h"
#include "Application.h"

ComponentTransform::ComponentTransform( GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::TRANSFORM, parent, active) {	UUID = (uint)App->GetRandomGenerator().Int(); }

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
#include "ComponentTransform.h"
#include "imgui.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform( GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::TRANSFORM, parent, active)
{
		
}

ComponentTransform::~ComponentTransform()
{}

void ComponentTransform::Update()
{
	UpdateTransform();
}

void ComponentTransform::CleanUp()
{}

void ComponentTransform::OnEditor() {

	if (ImGui::CollapsingHeader("Transform")) {

		ImGui::DragFloat3("Translation", &translation.x,0.05f);
		ImGui::DragFloat3("Scale", &scale.x, 0.05f);

		uiRotation = rotation.ToEulerXYZ();
		uiRotation *= 180 / pi;
		ImGui::DragFloat3("Rotation", &uiRotation.x, 0.5f);
		uiRotation /= 180 / pi;
		rotation = Quat::FromEulerXYZ(uiRotation.x, uiRotation.y, uiRotation.z);

		ImGui::NewLine();
	}
}

void ComponentTransform::UpdateTransform()
{
		localTransform = float4x4::FromTRS(translation, rotation, scale);
		if (parent!=nullptr)
			globalTransform = parent->transform->globalTransform * localTransform;
		UpdateRenderTransform();
		mustUpdate = false;
}

void ComponentTransform::UpdateRenderTransform()
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			renderTransform.M[i * 4 + j] = localTransform[j][i];
}
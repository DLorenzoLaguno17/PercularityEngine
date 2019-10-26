#include "ComponentTransform.h"

ComponentTransform::ComponentTransform( GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::TRANSFORM, parent, active)
{}

ComponentTransform::~ComponentTransform()
{}

void ComponentTransform::Update()
{}

void ComponentTransform::OnEditor() {

	if (ImGui::CollapsingHeader("Transform")) {

		//Scale
		float3 auxScale = transform.GetScale();
		float auxScale2[3] = { auxScale.x, auxScale.y,auxScale.z };
		ImGui::InputFloat3("Scale", auxScale2);

		//Position
		float3 auxTranslation = transform.TranslatePart();
		float auxTranslation2[3] = { auxTranslation.x,auxTranslation.y,auxTranslation.z };
		ImGui::InputFloat3("Position", auxTranslation2);

		//Angle
		/*float3 xAxis = transform.WorldX();
		float3 yAxis = transform.WorldY();
		float3 zAxis = transform.WorldZ();*/
		float3 auxRotation = transform.ToEulerXYZ();
		float auxRotation2[3] = { auxRotation.x ,auxRotation.y ,auxRotation.z };
		ImGui::InputFloat3("Rotation", auxRotation2);

		ImGui::NewLine();
	}
}
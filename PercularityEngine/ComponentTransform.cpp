#include "ComponentTransform.h"

ComponentTransform::ComponentTransform(COMPONENT_TYPE type, GameObject* parent, bool active) :
	Component(type, parent, active) 
{}

ComponentTransform::~ComponentTransform() {}

void ComponentTransform::OnEditor() {
	if (ImGui::CollapsingHeader("Transform")) {
		
	}
}
#ifndef _ComponentTransform_H_
#define _ComponentTransform_H_

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"
#include "imgui.h"

class ComponentTransform : public Component {
public:
	ComponentTransform(COMPONENT_TYPE type, GameObject* parent, bool active) :
		Component(type, parent, active) {}

	~ComponentTransform() {}

	// Methods
	void Update() {}
	void OnEditor() {
		if (ImGui::CollapsingHeader("Transform")) {
			ImGui::Checkbox("Enabled", &active);
		}
	}

public:
	float4x4 transform;
};

#endif // _ComponentTransform_H_
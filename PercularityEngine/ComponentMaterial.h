#ifndef _ComponentMaterial_H_
#define _ComponentMaterial_H_

#include "Component.h"
#include "imgui.h"

class ComponentMaterial : public Component {
public:
	ComponentMaterial(COMPONENT_TYPE type, GameObject* parent, bool active) :
		Component(type, parent, active) {}

	~ComponentMaterial() {}

	// Methods
	void Update() {}
	void OnEditor() {
		if (ImGui::CollapsingHeader("Material")) {
			ImGui::Checkbox("Enabled", &active);

			ImGui::Image((void*)texture, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::NewLine();
		}
	}

public:
	uint texture = 0;
	const char* tex_name;
};

#endif // _ComponentMaterial_H_
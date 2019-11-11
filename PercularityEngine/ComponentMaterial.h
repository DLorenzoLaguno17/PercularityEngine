#ifndef _ComponentMaterial_H_
#define _ComponentMaterial_H_

#include "Component.h"
#include <string>
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
			ImGui::Checkbox(tex_name.c_str(), &active);
			ImGui::Text("Size: %dx%d", height, width);
			ImGui::NewLine();

			ImGui::Image((void*)texture, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::NewLine();
		}
	}

	void CleanUp(){}

	static COMPONENT_TYPE GetComponentType() { return COMPONENT_TYPE::MATERIAL; }

public:
	uint texture = 0;
	long width = 0;
	long height = 0;
	std::string tex_name;
};

#endif // _ComponentMaterial_H_
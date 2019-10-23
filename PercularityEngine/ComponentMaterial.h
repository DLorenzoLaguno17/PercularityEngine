#ifndef _ComponentMaterial_H_
#define _ComponentMaterial_H_

#include "Component.h"

class ComponentMaterial : public Component {
public:
	ComponentMaterial(COMPONENT_TYPE type, GameObject* parent, bool active) :
		Component(type, parent, active) {}

	~ComponentMaterial() {}

	// Methods
	void Update() {}
	void Enable() { active = true; }
	void Disable() { active = false; }

	void OnEditor() {}

public:
	uint texture = 0;
};

#endif // _ComponentMaterial_H_
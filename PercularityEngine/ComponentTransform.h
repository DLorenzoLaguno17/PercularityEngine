#ifndef _ComponentTransform_H_
#define _ComponentTransform_H_

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"
#include "imgui.h"

class ComponentTransform : public Component {
public:

	ComponentTransform(GameObject* parent, bool active=true);

	~ComponentTransform();

	// Methods
	void Update();
	void OnEditor();
	void CleanUp() {}

public:

	float4x4 transform;
};

#endif // _ComponentTransform_H_
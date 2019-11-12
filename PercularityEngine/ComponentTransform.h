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
	void CleanUp();

	//Update transform
	void UpdateTransform();

public:

	float4x4 globalTransform	=float4x4::identity;
	float4x4 localTransform		=float4x4::identity;

	Quat rotation		=Quat::identity;

	float3 translation	=float3::zero;
	float3 scale		=float3::one;
};

#endif // _ComponentTransform_H_
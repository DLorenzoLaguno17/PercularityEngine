#ifndef _ComponentTransform_H_
#define _ComponentTransform_H_

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"

#include "glmath.h"

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

	void UpdateRenderTransform();

public:

	bool mustUpdate = true;

	float4x4 globalTransform	=float4x4::identity;
	float4x4 localTransform		=float4x4::identity;
	
	mat4x4 renderTransform;

	Quat rotation		=Quat::identity;

	float3 translation	=float3::zero;
	float3 scale		=float3::one;
};

#endif // _ComponentTransform_H_
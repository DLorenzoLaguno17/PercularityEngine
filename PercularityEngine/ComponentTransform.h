#ifndef _ComponentTransform_H_
#define _ComponentTransform_H_

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"

#include "glmath.h"

class ComponentTransform : public Component {
	friend class ComponentMesh;
public:

	ComponentTransform(GameObject* parent, bool active=true);

	~ComponentTransform() {}

	// Methods
	void Update() {}
	void CleanUp() {}
	void OnEditor();

	//Update transform
	void UpdateTransform();
	void UpdateRenderTransform(); //float4x4 -> mat4x4
	void SetPosition(float3 position);
	void Move(float3 positionIncrease);
	void Scale(float3 newScale);

private:

	float4x4 globalTransform = float4x4::identity;
	float4x4 localTransform	 = float4x4::identity;
	
	mat4x4 renderTransform;

	Quat rotation		= Quat::identity;
	float3 uiRotation	= float3::zero;
						  
	float3 translation	= float3::zero;
	float3 scale		= float3::one;

	// Backup values
	Quat lastRotation = Quat::identity;
	float3 lastTranslation = float3::zero;
	float3 lastScale = float3::one;
};

#endif // _ComponentTransform_H_
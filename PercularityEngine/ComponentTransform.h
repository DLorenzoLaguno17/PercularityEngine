#ifndef _ComponentTransform_H_
#define _ComponentTransform_H_

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"

#include "glmath.h"

class ComponentTransform : public Component {
	
public:

	ComponentTransform(GameObject* parent, bool active);

	// Destructor
	~ComponentTransform() {}

	// Methods
	void Update();
	void OnEditor();
	void CleanUp() {}

	const float4x4& GetLocalTransform() const { return localTransform; }
	const float4x4& GetGlobalTransform() const { return globalTransform; }
	const float3& GetEulerRotation() const { return eulerRotation; }
	const float3& GetTranslation() const { return translation; }
	const float3& GetScale() const { return scale; }

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);

private:
	//Update transform
	void UpdateTransform();
	void UpdateRenderTransform();//float4x4 -> mat4x4
	void SetPosition(float3 newPosition);
	void Move(float3 positionIncrease);
	void Scale(float3 scale_);
	void SetLocalTransform(float4x4 tranform);
	void SetEulerRotation(float3 eulerAngles);
	void SetScale(float3 newScale);
	void SetToZero();
	void UpdateEulerRotation();

public:
	mat4x4 renderTransform;

private:
	bool mustUpdate = true;

	float4x4 globalTransform = float4x4::identity;
	float4x4 localTransform = float4x4::identity;

	Quat rotation = Quat::identity;
	float3 eulerRotation = float3::zero;

	float3 translation = float3::zero;
	float3 scale = float3::one;

	float3 translationM;
	float3 rotationM;
	float3 scaleM;
};

#endif // _ComponentTransform_H_
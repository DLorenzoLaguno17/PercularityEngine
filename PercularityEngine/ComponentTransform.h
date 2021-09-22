#ifndef _ComponentTransform_H_
#define _ComponentTransform_H_

#include "Component.h"
#include "ModuleUndo.h"
#include "glmath.h"

class ComponentTransform : public Component 
{
public:
	ComponentTransform(GameObject* parent, bool active, ComponentTransform* reference);
	~ComponentTransform() {}

	// Methods
	void Update();
	void OnEditor();
	void CleanUp() {}

	// Getters
	const Quat& GetRotation() const { return rotation; }
	const float3& GetEulerRotation() const { return eulerRotation; }
	const float3& GetTranslation() const { return translation; }
	const float3& GetScale() const { return scale; }

	const float4x4& GetLocalTransform() const { return localTransform; }
	const float4x4& GetGlobalTransform() const { return globalTransform; }
	const mat4x4 GetGlobalGLTransform() const; // float4x4 -> mat4x4

	// Setters
	void SetScale(float3 newScale);
	void SetPosition(float3 newPosition);
	void SetRotation(Quat newRotation);
	void UpdateEulerRotation();

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);

	// Update transform
	void UpdateTransform();
	void Move(float3 positionIncrease);
	void Scale(float3 scale_);
	void SetGlobalTransform(float4x4 tranform);
	void SetGlobalTransform(mat4x4 tranform);
	void SetLocalTransform(float4x4 tranform);
	void SetLocalTransform(mat4x4 transform);//TEST
	void SetToZero();
	void SetEulerRotation(float3 eulerAngles);

public:
	// For undo and redo features
	bool firstTime = false;
	float3 lastTranslation = float3::zero;
	float3 lastScale = float3::one;
	Quat lastRotation = Quat::identity;

private:
	bool mustUpdate = true;

	float4x4 globalTransform = float4x4::identity;
	float4x4 localTransform = float4x4::identity;

	Quat rotation = Quat::identity;
	float3 eulerRotation = float3::zero;
	float3 translation = float3::zero;
	float3 scale = float3::one;

	float3 translationM = float3::zero;
	float3 rotationM = float3::zero;
	float3 scaleM = float3::one;
};

// ---------------------------------------------------
class TranslateGameObject : public Action
{
public:
	TranslateGameObject(float3 lastPosition, float3 newPosition, uint gameObject_uuid) :
		lastPosition(lastPosition), newPosition(newPosition), uuid(gameObject_uuid) {}

	void Undo() override;
	void Redo() override;

	float3 lastPosition;
	float3 newPosition;
	uint uuid;
};

class RotateGameObject : public Action
{
public:
	RotateGameObject(Quat lastRotation, Quat newRotation, uint gameObject_uuid) :
		lastRotation(lastRotation), newRotation(newRotation), uuid(gameObject_uuid) {}

	void Undo() override;
	void Redo() override;

	Quat lastRotation;
	Quat newRotation;
	uint uuid;
};

class ScaleGameObject : public Action
{
public:
	ScaleGameObject(float3 lastScale, float3 newScale, uint gameObject_uuid) :
		lastScale(lastScale), newScale(newScale), uuid(gameObject_uuid) {}

	void Undo() override;
	void Redo() override;

	float3 lastScale;
	float3 newScale;
	uint uuid;
};

#endif // _ComponentTransform_H_
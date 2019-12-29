#ifndef _ComponentRigidbody_H_
#define _ComponentRigidbody_H_

#include "Component.h"

class btRigidBody;
class btQuaternion;
class Module;

class ComponentRigidBody : public Component {
public:
	ComponentRigidBody(GameObject* parent, bool active);
	ComponentRigidBody(btRigidBody* body);
	~ComponentRigidBody() {}

	// Methods
	void Update() override;
	void OnEditor();
	void CleanUp() override;

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void RotateBody(btQuaternion rotationQuaternion);
	void CreateBody(btRigidBody* body);

	void OnUpdateTransform();

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);

	bool followObject = false;

	static COMPONENT_TYPE GetComponentType() { return COMPONENT_TYPE::RIGIDBODY; }

	float3 localPosition=float3::zero;


public:
	std::vector<Module*> collision_listeners;
	btRigidBody* body = nullptr;
	float mass = 0.0f;
	//SENSOR sensorType = NONE;

};

#endif // _ComponentRigidbody_H_
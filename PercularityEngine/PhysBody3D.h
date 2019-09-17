#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "ModulePhysics3D.h"

#include <list>

class btRigidBody;
class Module;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void RotateBody(btQuaternion rotationQuaternion);

public:
	std::list<Module*> collision_listeners;
	btRigidBody* body = nullptr;
	SENSOR sensorType = NONE;
};

#endif // __PhysBody3D_H__
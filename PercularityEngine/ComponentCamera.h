#ifndef _COMPONENT_CAMERA_H__
#define _COMPONENT_CAMERA_H__

#include "Component.h"
#include "glmath.h"


class ComponentCamera : public Component{
	
	friend class ModuleCamera3D;

public:
	ComponentCamera();
	~ComponentCamera();

	void Update(float dt);

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

private:
	void CalculateViewMatrix();

public:
	vec3 X, Y, Z, Position, Reference;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;


};

#endif
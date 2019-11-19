#ifndef _MODULE_CAMERA_H__
#define _MODULE_CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class GameObject;
class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void HandleUserInput(float dt);

	//Focus camera on a gameObject
	void FocusCameraOn(GameObject* object);

public:
	vec3 X, Y, Z, Position, Reference;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
	ComponentCamera* camera;
};

#endif
#ifndef _MODULE_CAMERA_H__
#define _MODULE_CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;
class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void CameraLookAround(float speed, float3 reference);
	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void HandleUserInput(float dt);

	//Focus camera on a gameObject
	void FocusCameraOn(GameObject* object);

private:
	float3 reference=float3(0,0,0);
	ComponentCamera* camera;
};

#endif
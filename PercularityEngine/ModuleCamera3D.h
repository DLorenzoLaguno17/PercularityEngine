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

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void CameraLookAround(float speed, float3 reference);
	void HandleUserInput(float dt);
	void OnClick(const vec2& normMousePos);

	const LineSegment* GetLastRay()const { return &lastRay; }

	// Focus camera on a gameObject
	void FocusCameraOn(GameObject* object);

	// Makes the camera look in a certain direction
	void LookAt(const vec3 &Spot);

public:
	ComponentCamera* camera = nullptr;

private:
	float3 reference = float3(0,0,0);
	LineSegment lastRay;
};

#endif
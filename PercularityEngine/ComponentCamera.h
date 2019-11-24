#ifndef _COMPONENT_CAMERA_H__
#define _COMPONENT_CAMERA_H__

#include "Component.h"
#include "glmath.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ComponentCamera : public Component{
	
	friend class ModuleCamera3D;

public:
	ComponentCamera(GameObject* parent=nullptr, bool active=false);
	~ComponentCamera();

	void Update();
	void OnEditor();
	void OnUpdateTransform();

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);

	//~~~~Frustum 
	//Getters
	float GetNearPlane() const;
	float GetFarPlane() const;
	float GetFOV() const;
	float GetAspectRatio() const;

	//Setters
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetFOV(float fov);
	void SetAspectRatio(float ar);
	//~~~~~~~~~~~~~~~~~~~~~~

	float* GetOpenGLViewMatrix();
	float* GetOpenGLProjectionMatrix();

	void DrawFrustum();
	void UpdatePlanes();

	static COMPONENT_TYPE GetComponentType() { return COMPONENT_TYPE::CAMERA; }

public:
	Plane planes[6];
	Frustum frustum;
	bool update_projection = true;

};

#endif
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

	void Update(float dt);

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

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

	void DrawFrustum();

private:
	void CalculateViewMatrix();

public:
	//Frustum

	Frustum frustum;
	bool update_projection = true;
	//frustum

	vec3 X, Y, Z, Position, Reference;


private:
	mat4x4 ViewMatrix, ViewMatrixInverse;


};

#endif
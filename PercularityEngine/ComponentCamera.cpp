#include "ComponentCamera.h"

#include "ModuleScene.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"


ComponentCamera::ComponentCamera():Component(COMPONENT_TYPE::CAMERA, nullptr, true)
{
	frustum.type = PerspectiveFrustum;
	frustum.pos = float3(0,0,0);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 0.5f;
	frustum.farPlaneDistance = 100.0f;

	frustum.verticalFov = 60.f*DEGTORAD;
	SetAspectRatio(1.5f);

	CalculateViewMatrix();

	X = vec3(0.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 0.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 0.0f);

	Position = vec3(0.0f, 0.0f, -5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	Move(vec3(20.0f, 0.0f, 0.0f));
	LookAt(vec3(0.0f, 0.0f, 0.0f));
}

ComponentCamera::~ComponentCamera()
{}


// -----------------------------------------------------------------
void ComponentCamera::Update(float dt)
{
	BROFILER_CATEGORY("CameraUpdate", Profiler::Color::LightSeaGreen);

	// Recalculate matrix -------------
	CalculateViewMatrix();

}

// -----------------------------------------------------------------
void ComponentCamera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ComponentCamera::LookAt(const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ComponentCamera::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ComponentCamera::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ComponentCamera::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

//------------------FRUSTUM-------------------

float ComponentCamera::GetNearPlane() const {
	
	return frustum.nearPlaneDistance;
}

float ComponentCamera::GetFarPlane()const
{
	return frustum.farPlaneDistance;
}

float ComponentCamera::GetFOV() const
{
	return frustum.verticalFov*RADTODEG;
}

float ComponentCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

void ComponentCamera::SetNearPlane(float distance)
{
	if (distance > 0.0f && distance < frustum.farPlaneDistance) {
		frustum.nearPlaneDistance = distance;
		update_projection = true;
	}
}

void ComponentCamera::SetFarPlane(float distance)
{
	if (distance >0.0f&&distance>frustum.nearPlaneDistance){
		frustum.farPlaneDistance = distance;
		update_projection = true;
	}
}

void ComponentCamera::SetFOV(float fov)
{
	float aspectRatio = frustum.AspectRatio();

	frustum.verticalFov = fov * DEGTORAD;
	SetAspectRatio(aspectRatio);

}

void ComponentCamera::SetAspectRatio(float ar)
{
	frustum.horizontalFov = 2.0f*atan(tan(frustum.verticalFov*0.5f)/ar);
	update_projection = true;

}
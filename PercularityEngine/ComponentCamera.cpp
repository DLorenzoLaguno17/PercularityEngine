#include "ComponentCamera.h"

#include "ComponentTransform.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Brofiler/Lib/Brofiler.h"
#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"
#include "OpenGL.h"


ComponentCamera::ComponentCamera(GameObject* parent, bool active):Component(COMPONENT_TYPE::CAMERA, parent, active)
{
	frustum.type = PerspectiveFrustum;
	frustum.pos = float3(0,0,-10.0f);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 0.5f;
	frustum.farPlaneDistance = 200.0f;

	frustum.verticalFov = 60.f*DEGTORAD;
	SetAspectRatio(1.0f);
}

ComponentCamera::~ComponentCamera() {}

// -----------------------------------------------------------------
void ComponentCamera::Update()
{
	BROFILER_CATEGORY("CameraUpdate", Profiler::Color::LightSeaGreen);

}

//------------------FRUSTUM-------------------

float ComponentCamera::GetNearPlane() const 
{
	
	return frustum.nearPlaneDistance;
}

float ComponentCamera::GetFarPlane() const
{
	return frustum.farPlaneDistance;
}

float ComponentCamera::GetFOV() const
{
	return frustum.verticalFov*RADTODEG;
}

float ComponentCamera::GetAspectRatio() const
{
	//return frustum.AspectRatio();

	return tanf(frustum.horizontalFov / 2) / tanf(frustum.verticalFov / 2);
}

void ComponentCamera::SetNearPlane(float distance)
{
	if (distance > 0.0f && distance < frustum.farPlaneDistance) {
		frustum.nearPlaneDistance = distance;
		update_projection = true;

		UpdatePlanes();
	}
}

void ComponentCamera::SetFarPlane(float distance)
{
	if (distance >0.0f&&distance>frustum.nearPlaneDistance){
		frustum.farPlaneDistance = distance;
		update_projection = true;

		UpdatePlanes();
	}
}

void ComponentCamera::SetFOV(float fov)
{
	float aspectRatio = frustum.AspectRatio();

	frustum.verticalFov = fov * DEGTORAD;
	SetAspectRatio(aspectRatio);

	UpdatePlanes();
}

void ComponentCamera::SetAspectRatio(float ar)
{
	frustum.horizontalFov = 2.0f * atanf( tanf(frustum.verticalFov*0.5f)*ar);

	UpdatePlanes();
	update_projection = true;
}

float* ComponentCamera::GetOpenGLViewMatrix()
{
	static float4x4 m;

	m = frustum.ViewMatrix();

	m.Transpose();

	return (float*)m.v;
}

float* ComponentCamera::GetOpenGLProjectionMatrix()
{
	static float4x4 m;

	m = frustum.ProjectionMatrix();

	m.Transpose();

	return (float*)m.v;
}

void ComponentCamera::DrawFrustum()
{
	glLineWidth(2.0f);
	this;

	glBegin(GL_LINES);

	glColor3f(1, 1, 1); //Light green color

	//Frustum lines
	glVertex3f(frustum.CornerPoint(0).x, frustum.CornerPoint(0).y, frustum.CornerPoint(0).z);
	glVertex3f(frustum.CornerPoint(1).x, frustum.CornerPoint(1).y, frustum.CornerPoint(1).z);
	
	glVertex3f(frustum.CornerPoint(2).x, frustum.CornerPoint(2).y, frustum.CornerPoint(2).z);
	glVertex3f(frustum.CornerPoint(3).x, frustum.CornerPoint(3).y, frustum.CornerPoint(3).z);

	glVertex3f(frustum.CornerPoint(4).x, frustum.CornerPoint(4).y, frustum.CornerPoint(4).z);
	glVertex3f(frustum.CornerPoint(5).x, frustum.CornerPoint(5).y, frustum.CornerPoint(5).z);

	glVertex3f(frustum.CornerPoint(6).x, frustum.CornerPoint(6).y, frustum.CornerPoint(6).z);
	glVertex3f(frustum.CornerPoint(7).x, frustum.CornerPoint(7).y, frustum.CornerPoint(7).z);

	//near plane
	glVertex3f(frustum.CornerPoint(0).x, frustum.CornerPoint(0).y, frustum.CornerPoint(0).z);
	glVertex3f(frustum.CornerPoint(2).x, frustum.CornerPoint(2).y, frustum.CornerPoint(2).z);

	glVertex3f(frustum.CornerPoint(2).x, frustum.CornerPoint(2).y, frustum.CornerPoint(2).z);
	glVertex3f(frustum.CornerPoint(6).x, frustum.CornerPoint(6).y, frustum.CornerPoint(6).z);

	glVertex3f(frustum.CornerPoint(4).x, frustum.CornerPoint(4).y, frustum.CornerPoint(4).z);
	glVertex3f(frustum.CornerPoint(0).x, frustum.CornerPoint(0).y, frustum.CornerPoint(0).z);

	glVertex3f(frustum.CornerPoint(6).x, frustum.CornerPoint(6).y, frustum.CornerPoint(6).z);
	glVertex3f(frustum.CornerPoint(4).x, frustum.CornerPoint(4).y, frustum.CornerPoint(4).z);

	//Far lines
	glVertex3f(frustum.CornerPoint(3).x, frustum.CornerPoint(3).y, frustum.CornerPoint(3).z);
	glVertex3f(frustum.CornerPoint(1).x, frustum.CornerPoint(1).y, frustum.CornerPoint(1).z);

	glVertex3f(frustum.CornerPoint(7).x, frustum.CornerPoint(7).y, frustum.CornerPoint(7).z);
	glVertex3f(frustum.CornerPoint(3).x, frustum.CornerPoint(3).y, frustum.CornerPoint(3).z);

	glVertex3f(frustum.CornerPoint(1).x, frustum.CornerPoint(1).y, frustum.CornerPoint(1).z);
	glVertex3f(frustum.CornerPoint(5).x, frustum.CornerPoint(5).y, frustum.CornerPoint(5).z);

	glVertex3f(frustum.CornerPoint(5).x, frustum.CornerPoint(5).y, frustum.CornerPoint(5).z);
	glVertex3f(frustum.CornerPoint(7).x, frustum.CornerPoint(7).y, frustum.CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1); //Blue color
}

void ComponentCamera::OnEditor()
{
	if (ImGui::CollapsingHeader("Camera")) {
		
		ImGui::DragFloat("Near plane distance", &frustum.nearPlaneDistance,1.0f,0.0f, frustum.farPlaneDistance);
		ImGui::DragFloat("Far plane distance", &frustum.farPlaneDistance, 1.0f, frustum.nearPlaneDistance, 2000);
		
		float verticalFov = frustum.verticalFov*RADTODEG;
		if (ImGui::DragFloat("FOV", &verticalFov))
			frustum.verticalFov = verticalFov*DEGTORAD;

		float horizontalFov = frustum.horizontalFov*RADTODEG;
		if (ImGui::DragFloat("Horizontal FOV", &horizontalFov))
			frustum.horizontalFov = horizontalFov * DEGTORAD;		
	}
}

void ComponentCamera::OnUpdateTransform()
{
	frustum.pos = gameObject->transform->GetTranslation();

	frustum.front = gameObject->transform->GetGlobalTransform().WorldZ();
	frustum.up = gameObject->transform->GetGlobalTransform().WorldY();

	UpdatePlanes();
}

void ComponentCamera::UpdatePlanes()
{
	frustum.GetPlanes(planes);
}

// Load & Save 
void ComponentCamera::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file) 
{
	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Camera"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Camera"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Camera"]["Active"];
}

void ComponentCamera::OnSave(const char* gameObjectNum, nlohmann::json &scene_file) 
{
	scene_file["Game Objects"][gameObjectNum]["Components"]["Camera"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Camera"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Camera"]["Active"] = active;
}
#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "Globals.h"
#include "GameObject.h"
#include "ComponentCamera.h"

#include "OpenGL.h"
#include "Par Shapes/par_shapes.h"
#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleCamera3D::~ModuleCamera3D()
{}


bool ModuleCamera3D::Init()
{

	LOG("ModuleCamera Init()");

	camera = new ComponentCamera();
	reference = camera->frustum.pos;
	App->renderer3D->camera = camera;

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	//App->renderer3D->camera = App->scene->frustumTest->GetComponent<ComponentCamera>();

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");
	RELEASE(camera);

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	BROFILER_CATEGORY("CameraUpdate", Profiler::Color::LightSeaGreen);

	HandleUserInput(dt);

	camera->Update();

	glBegin(GL_LINES);

	glColor3f(1, 1, 1); //Light green color

	//Frustum lines
	glVertex3f(lastRay.a.x, lastRay.a.y, lastRay.a.z);
	glVertex3f(lastRay.b.x, lastRay.b.y, lastRay.b.z);

	glEnd();

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::FocusCameraOn(GameObject* object)
{
	float3 boxCenter;
	float vertexToCenter;

	boxCenter = { object->aabb.CenterPoint().x,object->aabb.CenterPoint().y,object->aabb.CenterPoint().z };

	camera->frustum.pos = boxCenter;
	//camera->Reference = boxCenter;

	vertexToCenter = object->aabb.MinimalEnclosingSphere().r;

	camera->frustum.pos -= camera->frustum.front * vertexToCenter*2.5;
}

void ModuleCamera3D::HandleUserInput(float dt)
{
	float3	newPos(0, 0, 0);
	float speed = 10.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 20.0f * dt;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		if (App->scene->selected != nullptr && App->scene->selected != App->scene->GetRoot())
			FocusCameraOn(App->scene->selected);

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += camera->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= camera->frustum.front * speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camera->frustum.WorldRight() * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camera->frustum.WorldRight() * speed;

		CameraLookAround(speed*10, camera->frustum.pos);

		camera->frustum.pos += newPos;
		reference += newPos;
	}

	// Mouse motion ----------------
	if (App->input->IsMouseInsideWindow(App->gui->scene_window)) {

		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT) {
			newPos -= camera->frustum.WorldRight() * App->input->GetMouseXMotion() * speed;
			newPos += camera->frustum.up * App->input->GetMouseYMotion() * speed;

			camera->frustum.pos += newPos;
			reference += newPos;
		}
		// Scroll to zoom in and out
		if (App->input->GetMouseZ() > 0) {
			camera->frustum.pos += camera->frustum.front * speed * 12;
		}
		else if (App->input->GetMouseZ() < 0) {
			camera->frustum.pos -= camera->frustum.front * speed * 12;
		}
	}
}

void ModuleCamera3D::CameraLookAround(float speed, float3 reference)
{
	float dx = -App->input->GetMouseXMotion()*speed;
	float dy = -App->input->GetMouseYMotion()*speed;

	math::Quat rotationX = math::Quat::RotateAxisAngle(float3::unitY, dx * DEGTORAD);
	math::Quat rotationY = math::Quat::RotateAxisAngle(camera->frustum.WorldRight(), dy * DEGTORAD);
	math::Quat finalRotation = rotationX * rotationY;

	camera->frustum.up = finalRotation.Mul(camera->frustum.up).Normalized();
	camera->frustum.front = finalRotation.Mul(camera->frustum.front).Normalized();

	float distance = (camera->frustum.pos - reference).Length();
	camera->frustum.pos = reference + (-camera->frustum.front * distance);

	if (!reference.Equals(this->reference))
		this->reference = camera->frustum.pos + camera->frustum.front * (camera->frustum.pos).Length();
}

void ModuleCamera3D::OnClick(const vec2& normMousePos)
{

	lastRay = App->renderer3D->GetCamera()->frustum.UnProjectLineSegment(normMousePos.x, normMousePos.y);

}
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "ComponentCamera.h"
#include "ModuleRenderer3D.h"

#include "Par Shapes/par_shapes.h"
#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	camera = new ComponentCamera();

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	App->renderer3D->camera = camera;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	BROFILER_CATEGORY("CameraUpdate", Profiler::Color::LightSeaGreen);

	HandleUserInput(dt);

	camera->Update(dt);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	camera->Look(Position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	camera->LookAt(Spot);
}

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	camera->Move(Movement);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}


void ModuleCamera3D::FocusCameraOn(GameObject* object)
{
	vec3 boxCenter;
	float vertexToCenter;

	boxCenter = { object->aabb.CenterPoint().x,object->aabb.CenterPoint().y,object->aabb.CenterPoint().z };

	camera->Position = boxCenter;
	camera->Reference = boxCenter;

	vertexToCenter = object->aabb.MinimalEnclosingSphere().r;

	camera->Position += camera->Z * vertexToCenter*1.5;
}

void ModuleCamera3D::HandleUserInput(float dt)
{
	vec3 newPos(0, 0, 0);
	float speed = 4.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		if (App->scene->selected != App->scene->GetRoot())
			FocusCameraOn(App->scene->selected);

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= camera->Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += camera->Z * speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= camera->X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += camera->X * speed;

		camera->Position += newPos;
		camera->Reference += newPos;
	}

	// Mouse motion ----------------
	if (App->input->IsMouseInsideWindow(App->gui->scene_window)) {

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			float Sensitivity = 0.25f;

			camera->Position -= camera->Reference;

			if (dx != 0)
			{
				float DeltaX = (float)dx * Sensitivity;

				camera->X = rotate(camera->X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				camera->Y = rotate(camera->Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				camera->Z = rotate(camera->Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			}

			if (dy != 0)
			{
				float DeltaY = (float)dy * Sensitivity;

				camera->Y = rotate(camera->Y, DeltaY, camera->X);
				camera->Z = rotate(camera->Z, DeltaY, camera->X);

				if (Y.y < 0.0f)
				{
					camera->Z = vec3(0.0f, camera->Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
					camera->Y = cross(camera->Z, camera->X);
				}
			}

			camera->Position = camera->Reference + camera->Z * length(camera->Position);
			camera->Position;
		}

		// Scroll to zoom in and out
		if (App->input->GetMouseZ() > 0) {
			camera->Position -= camera->Z * speed * 12;
		}
		else if (App->input->GetMouseZ() < 0) {
			camera->Position += camera->Z * speed * 12;
		}
	}
}
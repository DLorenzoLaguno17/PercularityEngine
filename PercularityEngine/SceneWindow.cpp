#include "SceneWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "mmgr/mmgr.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"


SceneWindow::SceneWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void SceneWindow::Update() {

	//Imgui
	ImGui::Begin("Scene", &active, ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoScrollWithMouse);

	windowPosition.x = ImGui::GetWindowPos().x;
	windowPosition.y = ImGui::GetWindowPos().y;
	windowSize.x = ImGui::GetContentRegionAvail().x;
	windowSize.y = ImGui::GetContentRegionAvail().y;

	// Check if the scene window has been resized
	if (windowSize.x != last_windowSize.x || windowSize.y != last_windowSize.y)
	{
		last_windowSize = windowSize;
		
	}
	
	ImGui::Image((void*)App->renderer3D->GetTexColorBuffer(), ImVec2(windowSize.x, windowSize.x/App->renderer3D->GetCamera()->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN &&
		(App->input->GetMouseX() > windowPosition.x && App->input->GetMouseX() < windowPosition.x + windowSize.x) &&
		(App->input->GetMouseY() > windowPosition.y && App->input->GetMouseY() < windowPosition.y + windowSize.y))
	{
		OnClick();

		
	}
}

void SceneWindow::OnClick()
{
	//WARNING!! HARDCODE ALERT: There's a margin in both axes, between the window border and the texture (in both axis)
		//and that made the ray tracing lose precision, that's why I made slight substractions in the mousePos values (-5,-30)
	vec2 mousePos(float(App->input->GetMouseX() - windowPosition.x - 5), float(App->input->GetMouseY() - windowPosition.y - 30));
	mousePos.x = -1.0 + 2.0f*(mousePos.x / windowSize.x);
	mousePos.y = 1.0 - 2.0f*(mousePos.y / (windowSize.x / App->renderer3D->GetCamera()->GetAspectRatio()));

	App->camera->OnClick(mousePos);

	App->scene->selected = SelectObject();
}

GameObject* SceneWindow::SelectObject() const
{
	GameObject* retItem = nullptr;

	LineSegment ray = *App->camera->GetLastRay();

	std::map<float,const GameObject*> objects;
	App->scene->sceneTree->CollectChilldren(ray,objects);

	std::map<float, const GameObject*>::iterator it;
	for (it = objects.begin(); it != objects.end(); ++it)
	{
		ComponentMesh* mesh = (ComponentMesh*)it->second->GetComponent(COMPONENT_TYPE::MESH);
		if (mesh)
		{
			LineSegment localRay = ray;
			localRay.Transform(it->second->transform->GetGlobalTransform().Inverted());
			for (uint v = 0; v < mesh->mesh.num_indices; v += 3)
			{
				uint indexA = mesh->mesh.indices[v] * 3;
				float3 a(mesh->mesh.vertices[indexA]);

				uint indexB = mesh->mesh.indices[v+1] * 3;
				float3 b(mesh->mesh.vertices[indexB]);

				uint indexC = mesh->mesh.indices[v+2] * 3;
				float3 c(mesh->mesh.vertices[indexC]);

				Triangle triangle(a, b, c);

				if (localRay.Intersects(triangle, nullptr, nullptr))
				{
					return (GameObject*)it->second;
				}
			}
		}
	}
	
	return retItem;
}


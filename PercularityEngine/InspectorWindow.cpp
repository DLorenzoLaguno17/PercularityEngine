#include "InspectorWindow.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModulePhysics.h"

#include <string>
#include "mmgr/mmgr.h"

InspectorWindow::InspectorWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void InspectorWindow::Update() {

	ImGui::Begin("Inspector", &active);

	if (App->scene->selected != nullptr) {
		ImGui::InputText("GameObject", (char*)App->scene->selected->name.c_str(), 20);

		if (ImGui::Checkbox("Static", &App->scene->selected->isStatic))
			App->scene->selected->MakeStatic(App->scene->selected->isStatic);

		ImGui::NewLine();
		App->scene->selected->OnEditor();

		if (App->scene->selected->GetComponent(COMPONENT_TYPE::RIGIDBODY) == nullptr)
		{
			if (ImGui::Button("Add Box RigidBody"))
				App->physics->AddRigidBody(App->scene->selected->obb, App->scene->selected, 10.0f);

			if (ImGui::Button("Add Sphere RigidBody")) {
				Sphere sphere; sphere.r = 2.5;
				App->physics->AddRigidBody(sphere, App->scene->selected, 10.0f);
			}
			
			if (ImGui::Button("Add Capsule RigidBody")) {
				LineSegment l = { App->scene->selected->obb.pos / 2, -App->scene->selected->obb.pos / 2 };
				Capsule capsule; capsule.r = 2.5; capsule.l = l;
				App->physics->AddRigidBody(capsule, App->scene->selected, 10.0f);
			}
		}		
	}

	ImGui::End();
}
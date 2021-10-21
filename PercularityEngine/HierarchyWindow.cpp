#include "HierarchyWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "mmgr/mmgr.h"

HierarchyWindow::HierarchyWindow(char* name, bool active) 
	: UIElement(name, active) {}

// Show scene window
void HierarchyWindow::Update() 
{
	ImGui::Begin("Hierarchy", &active);	
	DrawHierarchy(App->scene->GetRoot());

	ImGui::End();
}

void HierarchyWindow::DrawHierarchy(GameObject* root) 
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
		| (root->extended ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);

	if (root == App->scene->selected)
		node_flags |= ImGuiTreeNodeFlags_Selected;
	else
	{
		for (uint i = 0; i < App->scene->selectedNodes.size(); ++i)
		{
			if (root == App->scene->selectedNodes[i])
				node_flags |= ImGuiTreeNodeFlags_Selected;
			else
				node_flags |= ImGuiTreeNodeFlags_None;
		}
	}

	ImGui::PushID(root);

	if (root->children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	root->extended = ImGui::TreeNodeEx(root, node_flags, root->name.c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		uint uuid = root->GetUUID();
		ImGui::SetDragDropPayload("ChildUUID", &uuid, sizeof(uint));

		for (int i = 0; i < App->scene->selectedNodes.size(); ++i)
			ImGui::Text(App->scene->selectedNodes[i]->name.c_str());

		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ChildUUID"))
		{
			uint uuid = *(uint*)payload->Data;
			GameObject* dragged = App->scene->GetGameObject(App->scene->GetRoot(), uuid);

			// If we drag an unselected item, it becomes the selected one and we deselect the rest
			bool found = false;
			for (int i = 0; i < App->scene->selectedNodes.size(); ++i)
			{
				if (App->scene->selectedNodes[i] == dragged)
					found = true;
			}

			if (!found)
			{
				App->scene->selectedNodes.clear();
				App->scene->selected = dragged;
				App->scene->selectedNodes.push_back(dragged);
			}

			std::vector<uint> uuids, lastParents, newParents;
			std::vector<float3> lastPositions, newPositions, lastScales, newScales;
			std::vector<Quat> lastRotations, newRotations;

			for (int i = 0; i < App->scene->selectedNodes.size(); ++i)
			{
				GameObject* dragged = App->scene->selectedNodes[i];

				// Ensure we not reparent the game object with one of its children
				if (dragged != App->scene->GetRoot() && App->scene->GetGameObject(dragged, root->GetUUID()) == nullptr)
				{
					uuids.push_back(dragged->GetUUID());
					lastParents.push_back(dragged->parent_UUID);
					newParents.push_back(root->GetUUID());

					lastPositions.push_back(dragged->transform->GetTranslation());
					lastScales.push_back(dragged->transform->GetScale());
					lastRotations.push_back(dragged->transform->GetRotation());

					dragged->MakeChild(root);
					dragged->transform->RecalculateTransform(root->transform);

					newPositions.push_back(dragged->transform->GetTranslation());
					newScales.push_back(dragged->transform->GetScale());
					newRotations.push_back(dragged->transform->GetRotation());
				}
			}
			
			if (uuids.size() > 0)
			{
				ReparentGameObject* reparentAction = new ReparentGameObject(uuids, lastParents, newParents, lastPositions, newPositions, lastScales, newScales, lastRotations, newRotations);
				App->undo->StoreNewAction(reparentAction);
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked())
		root->clicked = true;

	if (ImGui::IsItemHovered() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && root->clicked == true)
	{
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT)
		{
			App->scene->selectedNodes.clear();
			App->scene->selected = root;
		}

		App->scene->selectedNodes.push_back(root);
	}

	if (root->extended) 
	{
		for (int i = 0; i < root->children.size(); ++i)
			DrawHierarchy(root->children[i]);
		
		if (!(node_flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePop();
	}

	ImGui::PopID();
}

void ReparentGameObject::Undo()
{
	for (int i = 0; i < uuids.size(); ++i)
	{
		GameObject* gameObject = App->scene->GetGameObject(App->scene->GetRoot(), uuids[i]);
		GameObject* parent = App->scene->GetGameObject(App->scene->GetRoot(), lastParent_uuids[i]);
		gameObject->MakeChild(parent);

		gameObject->transform->SetPosition(lastPositions[i]);
		gameObject->transform->SetScale(lastScales[i]);
		gameObject->transform->SetRotation(lastRotations[i]);

		gameObject->transform->lastTranslation = gameObject->transform->GetTranslation();
		gameObject->transform->lastRotation = gameObject->transform->GetRotation();
		gameObject->transform->lastScale = gameObject->transform->GetScale();
	}
}

void ReparentGameObject::Redo()
{
	for (int i = 0; i < uuids.size(); ++i)
	{
		GameObject* gameObject = App->scene->GetGameObject(App->scene->GetRoot(), uuids[i]);
		GameObject* parent = App->scene->GetGameObject(App->scene->GetRoot(), newParent_uuids[i]);
		gameObject->MakeChild(parent);

		gameObject->transform->SetPosition(newPositions[i]);
		gameObject->transform->SetScale(newScales[i]);
		gameObject->transform->SetRotation(newRotations[i]);

		gameObject->transform->lastTranslation = gameObject->transform->GetTranslation();
		gameObject->transform->lastRotation = gameObject->transform->GetRotation();
		gameObject->transform->lastScale = gameObject->transform->GetScale();
	}
}

void ReparentGameObject::CleanUp() 
{
	uuids.clear();
	lastParent_uuids.clear();
	newParent_uuids.clear();
}
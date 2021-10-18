#include "HierarchyWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

HierarchyWindow::HierarchyWindow(char* name, bool active) 
	: UIElement(name, active) {}

// Show scene window
void HierarchyWindow::Update() 
{
	ImGui::Begin("Hierarchy", &active);	
	DrawHierarchy(App->scene->GetRoot());

	ImGui::End();

	if (!App->scene->selected)
		selectedNodes.clear();
}

void HierarchyWindow::DrawHierarchy(GameObject* root) 
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
		| (root->extended ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);

	if (root == App->scene->selected)
		node_flags |= ImGuiTreeNodeFlags_Selected;
	else
	{
		for (uint i = 0; i < selectedNodes.size(); ++i)
		{
			if (root == selectedNodes[i])
				node_flags |= ImGuiTreeNodeFlags_Selected;
			else
				node_flags |= ImGuiTreeNodeFlags_None;
		}
	}

	ImGui::PushID(root);
	int a = root->children.size();

	if (root->children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	root->extended = ImGui::TreeNodeEx(root, node_flags, root->name.c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		uint uuid = root->GetUUID();
		ImGui::SetDragDropPayload("ChildUUID", &uuid, sizeof(uint));
		ImGui::Text(root->name.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ChildUUID"))
		{
			GameObject* dragged = App->scene->GetGameObject(App->scene->GetRoot(), *(uint*)payload->Data);
			ReparentGameObject* reparentAction = new ReparentGameObject(dragged->GetUUID(), dragged->parent_UUID, root->GetUUID());
			App->undo->StoreNewAction(reparentAction);
			dragged->MakeChild(root);
		}

		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked())
	{
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT)
		{
			selectedNodes.clear();
			App->scene->selected = root;
		}

		selectedNodes.push_back(root);
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

// --------------------------------------------------
void ReparentGameObject::Undo()
{
	GameObject* gameObject = App->scene->GetGameObject(App->scene->GetRoot(), uuid);
	GameObject* parent = App->scene->GetGameObject(App->scene->GetRoot(), lastParent_uuid);
	gameObject->MakeChild(parent);
}

void ReparentGameObject::Redo()
{
	GameObject* gameObject = App->scene->GetGameObject(App->scene->GetRoot(), uuid);
	GameObject* parent = App->scene->GetGameObject(App->scene->GetRoot(), newParent_uuid);
	gameObject->MakeChild(parent);
}

void ReparentGameObject::CleanUp() {}
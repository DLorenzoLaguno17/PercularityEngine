#include "HierarchyWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

HierarchyWindow::HierarchyWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void HierarchyWindow::Update() {

	ImGui::Begin("Hierarchy", &active);	
	DrawHierarchy(App->scene->root);
	ImGui::End();
}

void HierarchyWindow::DrawHierarchy(GameObject* root) {

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | (root->extended ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None)
		| ImGuiTreeNodeFlags_OpenOnDoubleClick | (App->scene->selected == root ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None);

	ImGui::PushID(root);

	if (root->children.empty())
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	root->extended = ImGui::TreeNodeEx(root, node_flags, root->name.c_str());
	
	if (ImGui::IsItemClicked())
	{
		App->scene->selected = root;
	}

	if (root->extended) {
		for (int i = 0; i < root->children.size(); ++i)
			DrawHierarchy(root->children[i]);
		
		if (!(node_flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePop();
	}

	ImGui::PopID();
}
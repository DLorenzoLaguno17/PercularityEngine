#include "HierarchyWindow.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

HierarchyWindow::HierarchyWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void HierarchyWindow::Update() {
	ImGui::Begin("Hierarchy", &active);

	/*if (ImGui::TreeNode("Scene"))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static bool align_label_with_current_x_position = false;
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanAvailWidth);
		ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
		ImGui::Checkbox("Align label with current X position)", &align_label_with_current_x_position);
		ImGui::Text("Hello!");
		if (align_label_with_current_x_position)
			ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

		static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
		int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
		for (int i = 0; i < 6; i++)
		{
			// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (selection_mask & (1 << i)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			if (i < 3)
			{
				// Items 0..2 are Tree Node
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
				if (ImGui::IsItemClicked())
					node_clicked = i;
				if (node_open)
				{
					ImGui::BulletText("Blah blah\nBlah Blah");
					ImGui::TreePop();
				}
			}
			else
			{
				// Items 3..5 are Tree Leaves
				// The only reason we use TreeNode at all is to allow selection of the leaf.
				// Otherwise we can use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
				node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
				if (ImGui::IsItemClicked())
					node_clicked = i;
			}
		}
		if (node_clicked != -1)
		{
			// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
				selection_mask = (1 << node_clicked);           // Click to single-select
		}
		if (align_label_with_current_x_position)
			ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::TreePop();*/

	//if (ImGui::TreeNode("Scene")) {
		for (int i = 0; i < App->scene->game_objects.size(); ++i) {		

			if (ImGui::Selectable(App->scene->game_objects[i].name.c_str(), App->scene->selected_id == i)) {
				App->scene->selected_id = i;
				App->scene->selected = &App->scene->game_objects[i];

				//if (ImGui::TreeNode("Test")) { ImGui::TreePop(); }
			}
		
		}

	//	ImGui::TreePop();
//	}
	//RecursiveTree(App->scene->root);

	ImGui::End();
}

void HierarchyWindow::RecursiveTree(GameObject* root) {

	if (ImGui::TreeNode(root->name.c_str())) {
		for (int i = 0; i < App->scene->game_objects.size(); ++i) {

			if (ImGui::Selectable(App->scene->game_objects[i].name.c_str(), App->scene->selected_id == i)) {
				App->scene->selected_id = i;
				App->scene->selected = &App->scene->game_objects[i];
			}

			RecursiveTree(root->children[i]);
		}

		ImGui::TreePop();
	}
}

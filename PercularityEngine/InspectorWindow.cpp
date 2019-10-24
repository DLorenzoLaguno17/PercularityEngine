#include "InspectorWindow.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

#include "mmgr/mmgr.h"

InspectorWindow::InspectorWindow(char* name, bool active) : UIElement(name, active) {}

// Show scene window
void InspectorWindow::Update() {

	ImGui::Begin("Inspector", &active);

	if (App->scene->selected != nullptr) {
		ImGui::TextColored(ImVec4(0, 128, 128, 255), App->scene->selected->name.c_str());		

		//if (ImGui::TreeNode("Multi-line Text Input"))
  //      {
  //          // Note: we are using a fixed-sized buffer for simplicity here. See ImGuiInputTextFlags_CallbackResize
  //          // and the code in misc/cpp/imgui_stdlib.h for how to setup InputText() for dynamically resizing strings.
  //          static char text[1024 * 16] =
  //              "/*\n"
  //              " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
  //              " the hexadecimal encoding of one offending instruction,\n"
  //              " more formally, the invalid operand with locked CMPXCHG8B\n"
  //              " instruction bug, is a design flaw in the majority of\n"
  //              " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
  //              " processors (all in the P5 microarchitecture).\n"
  //              "*/\n\n"
  //              "label:\n"
  //              "\tlock cmpxchg8b eax\n";

		//          static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
  //          HelpMarker("You can use the ImGuiInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/imgui_stdlib.h for an example. (This is not demonstrated in imgui_demo.cpp)");
  //          ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", (unsigned int*)&flags, ImGuiInputTextFlags_ReadOnly);
  //          ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", (unsigned int*)&flags, ImGuiInputTextFlags_AllowTabInput);
  //          ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", (unsigned int*)&flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
  //          ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
  //          ImGui::TreePop();
  //      }

		ImGui::NewLine();
		App->scene->selected->OnEditor();
	}

	ImGui::End();
}
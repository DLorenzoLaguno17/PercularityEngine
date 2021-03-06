#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleScene.h"
#include "DebugWindow.h"
#include "Debug.h"

DebugWindow::DebugWindow(char* name, bool active) : 
	UIElement(name, active) {}

void DebugWindow::Update()
{
	ImGui::Begin("Debug functionalities", &active);

	ImGui::Text("Physics");
	if (ImGui::Checkbox("Activate constraint", &activateConstraint))
	{
		if (activateConstraint) App->physics->CreateTestConstraint();
		else App->physics->DeleteTestConstraint();
	}
	if (ImGui::Checkbox("Show rigidbodies", &debugPhysics))
		App->physics->debug = !App->physics->debug;

	if (ImGui::Button("Physicalize the scene"))
		App->physics->PhysicalizeScene(App->scene->GetRoot());

	ImGui::NewLine();
	ImGui::Text("Scene");
	bool drawOCTREE = Debug::drawSceneTree;
	if (ImGui::Checkbox("Draw scene Octree", &drawOCTREE))
		Debug::drawSceneTree = drawOCTREE;
	
	bool drawSceneAxis = Debug::drawSceneAxis;
	if (ImGui::Checkbox("Draw axis", &drawSceneAxis))
		Debug::drawSceneAxis = drawSceneAxis;
	
	bool drawScenePlane = Debug::drawScenePlane;
	if (ImGui::Checkbox("Draw plane", &drawScenePlane))
		Debug::drawScenePlane = drawScenePlane;

	ImGui::NewLine();
	ImGui::Text("Game Objects");
	bool drawAABBS = Debug::drawObjectsAABB;
	if (ImGui::Checkbox("Draw AABBs", &drawAABBS))
		Debug::drawObjectsAABB = drawAABBS;

	ImGui::NewLine();
	ImGui::Text("Rendering");
	bool frustumCulling = Debug::frustumCullingActive;
	if (ImGui::Checkbox("Frustum culling", &frustumCulling))
		Debug::frustumCullingActive= frustumCulling;

	bool cullingAccelerated = Debug::frustumAccelerated;
	if (ImGui::Checkbox("Culling accelerated", &cullingAccelerated))
		Debug::frustumAccelerated = cullingAccelerated;

	ImGui::NewLine();
	ImGui::Text("Mouse");

	bool showMouseRay = Debug::drawMouseLine;
	if (ImGui::Checkbox("Mouse ray", &showMouseRay))
		Debug::drawMouseLine = showMouseRay;



	ImGui::NewLine();
	ImGui::Text("Colliders");

	bool drawColliders = Debug::drawColliders;
	if (ImGui::Checkbox("Show colliders", &drawColliders))
		Debug::drawColliders = drawColliders;

	ImGui::End();
}

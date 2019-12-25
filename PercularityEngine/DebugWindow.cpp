#include "DebugWindow.h"
#include "Debug.h"

DebugWindow::DebugWindow(char* name, bool active): UIElement(name, active)
{
}

void DebugWindow::Update()
{
	ImGui::Begin("Debug functionalities", &active);

	ImGui::Text("");
	ImGui::Text("Scene");
	bool drawOCTREE = Debug::drawSceneTree;
	if (ImGui::Checkbox("Draw scene Octree", &drawOCTREE))
		Debug::drawSceneTree =drawOCTREE;
	
	bool drawSceneAxis = Debug::drawSceneAxis;
	if (ImGui::Checkbox("Draw axis", &drawSceneAxis))
		Debug::drawSceneAxis = drawSceneAxis;
	
	bool drawScenePlane = Debug::drawScenePlane;
	if (ImGui::Checkbox("Draw plane", &drawScenePlane))
		Debug::drawScenePlane = drawScenePlane;

	ImGui::Text("");
	ImGui::Text("Game Objects");
	bool drawAABBS = Debug::drawObjectsAABB;
	if (ImGui::Checkbox("Draw AABBs", &drawAABBS))
		Debug::drawObjectsAABB=drawAABBS;


	ImGui::Text("");
	ImGui::Text("Rendering");
	bool frustumCulling = Debug::frustumCullingActive;
	if (ImGui::Checkbox("Frustum culling", &frustumCulling))
		Debug::frustumCullingActive= frustumCulling;

	bool cullingAccelerated = Debug::frustumAccelerated;
	if (ImGui::Checkbox("Culling accelerated", &cullingAccelerated))
		Debug::frustumAccelerated= cullingAccelerated;



	ImGui::End();
}
#ifndef __HierarchyWindow_H__
#define __HierarchyWindow_H__

#include "UIElement.h"
#include "ModuleUndo.h"

class GameObject;

class ReparentGameObject : public Action
{
public:
	ReparentGameObject(std::vector<uint> uuids, std::vector<uint> lastParents, std::vector<uint> newParents, std::vector<float3> lastPositions, std::vector<float3> newPositions, std::vector<float3> lastScales, std::vector<float3> newScales, std::vector<Quat> lastRotations, std::vector<Quat> newRotations)
		: uuids(uuids), lastParent_uuids(lastParents), newParent_uuids(newParents), lastPositions(lastPositions), newPositions(newPositions), lastScales(lastScales), newScales(newScales), lastRotations(lastRotations), newRotations(newRotations) {}

	void Undo() override;
	void Redo() override;
	void CleanUp() override;

private:
	std::vector<uint> uuids;
	std::vector<uint> lastParent_uuids;
	std::vector<uint> newParent_uuids;

	std::vector<float3> lastPositions;
	std::vector<float3> newPositions;

	std::vector<float3> lastScales;
	std::vector<float3> newScales;

	std::vector<Quat> lastRotations;
	std::vector<Quat> newRotations;
};

// ---------------------------------------------------
class HierarchyWindow : public UIElement
{
public:
	HierarchyWindow(char* name, bool active = true);

	// Destructor
	virtual ~HierarchyWindow() {};

	// Called every frame
	void Update();

private:
	void DrawHierarchy(GameObject* root);
};

#endif // __HierarchyWindow_H__
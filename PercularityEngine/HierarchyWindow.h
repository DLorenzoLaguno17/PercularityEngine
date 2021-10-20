#ifndef __HierarchyWindow_H__
#define __HierarchyWindow_H__

#include "UIElement.h"
#include "ModuleUndo.h"

class GameObject;

class ReparentGameObject : public Action
{
public:
	ReparentGameObject(std::vector<uint> uuids, std::vector<uint> lastParents, std::vector<uint> newParents)
		: uuids(uuids), lastParent_uuids(lastParents), newParent_uuids(newParents) {}

	void Undo() override;
	void Redo() override;
	void CleanUp() override;

private:
	std::vector<uint> uuids;
	std::vector<uint> lastParent_uuids;
	std::vector<uint> newParent_uuids;
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
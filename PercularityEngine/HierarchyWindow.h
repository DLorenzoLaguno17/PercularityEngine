#ifndef __HierarchyWindow_H__
#define __HierarchyWindow_H__

#include "UIElement.h"
#include "ModuleUndo.h"

class GameObject;

class ReparentGameObject : public Action
{
public:
	ReparentGameObject(uint uuid, uint lastParent, uint newParent)
		: uuid(uuid), lastParent_uuid(lastParent), newParent_uuid(newParent) {}

	void Undo() override;
	void Redo() override;
	void CleanUp() override;

private:
	uint uuid = 0;
	uint lastParent_uuid = 0;
	uint newParent_uuid = 0;
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

private:
	std::vector<GameObject*> selectedNodes;
};

#endif // __HierarchyWindow_H__
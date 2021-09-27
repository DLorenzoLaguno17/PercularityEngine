#ifndef __SCENEACTIONS_H__
#define __SCENEACTIONS_H__

#include "ModuleUndo.h"

class GameObject;

class CreateGameObject : public Action
{
public:
	CreateGameObject(GameObject* gameObject);

	void Undo() override;
	void Redo() override;
	void CleanUp() override;

private:
	uint uuid = 0;
	GameObject* parent = nullptr;
	GameObject* backup = nullptr;
};

class DeleteGameObject : public Action
{
public:
	DeleteGameObject(GameObject* gameObject, GameObject* parent);

	void Undo() override;
	void Redo() override;
	void CleanUp() override;

private:
	uint uuid = 0;
	GameObject* parent = nullptr;
	GameObject* backup = nullptr;
};

#endif // __SCENEACTIONS_H__
#include "Application.h"
#include "ModuleScene.h"
#include "SceneActions.h"
#include "GameObject.h"

CreateGameObject::CreateGameObject(GameObject* gameObject) :
	uuid(gameObject->GetUUID()), parent(gameObject->parent)
{
	backup = new GameObject(gameObject);
};

void CreateGameObject::Undo()
{
	GameObject* toDelete = App->scene->GetGameObject(App->scene->GetRoot(), uuid);
	App->scene->RemoveGameObject(toDelete);
}

void CreateGameObject::Redo()
{
	GameObject* newGo = new GameObject(backup);
	newGo->MakeChild(parent);
	App->scene->selected = newGo;
}

void CreateGameObject::CleanUp()
{
	App->scene->RecursiveCleanUp(backup);
}

// -------------------------------------------------------------------------
DeleteGameObject::DeleteGameObject(GameObject* gameObject, GameObject* parent) :
	uuid(gameObject->GetUUID()), backup(gameObject), parent(parent) {};

void DeleteGameObject::Undo()
{
	GameObject* newGo = new GameObject(backup);
	newGo->MakeChild(parent);
	App->scene->selected = newGo;
}

void DeleteGameObject::Redo()
{
	GameObject* toDelete = App->scene->GetGameObject(App->scene->GetRoot(), uuid);
	App->scene->RemoveGameObject(toDelete);
}

void DeleteGameObject::CleanUp()
{
	App->scene->RecursiveCleanUp(backup);
}
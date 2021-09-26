#include "Application.h"
#include "ModuleScene.h"
#include "SceneActions.h"
#include "GameObject.h"

CreateGameObject::CreateGameObject(GameObject* gameObject) :
	gameObject(gameObject), parent(gameObject->parent)
{
	backup = new GameObject(gameObject);
};

void CreateGameObject::Undo()
{
	App->scene->RemoveGameObject(gameObject);
}

void CreateGameObject::Redo()
{
	gameObject = new GameObject(backup);
	gameObject->MakeChild(parent);
	App->scene->selected = gameObject;
}

void CreateGameObject::CleanUp()
{
	App->scene->RecursiveCleanUp(backup);
}

void DeleteGameObject::Undo()
{
	gameObject = new GameObject(backup);
	gameObject->MakeChild(parent);
	App->scene->selected = gameObject;
}

void DeleteGameObject::Redo()
{
	App->scene->RemoveGameObject(gameObject);
}

void DeleteGameObject::CleanUp()
{
	App->scene->RecursiveCleanUp(backup);
}
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ResourceMesh.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModulePhysics.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "Debug.h"

#include "OpenGL.h"
#include "glmath.h"
#include "mmgr/mmgr.h"

GameObject::GameObject() {
	name = "Untitled";
	MakeChild(App->scene->GetRoot());
	UUID = (uint)App->GetRandomGenerator().Int();
	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);

	App->scene->nonStaticObjects.push_back(this);
}

GameObject::GameObject(std::string name, GameObject* parent, bool loadingScene) : name(name)
{
	if (parent) MakeChild(parent);
	
	if (strcmp("World", name.c_str()) != 0)
		UUID = (uint)App->GetRandomGenerator().Int();

	if (!loadingScene) transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);

	App->scene->nonStaticObjects.push_back(this);
}

// Generates a new UUID
void GameObject::NewUUID() 
{ 
	UUID = (uint)App->GetRandomGenerator().Int(); 
}

// Called every frame
void GameObject::Update() {
	if (Debug::drawObjectsAABB || showBondingBox)
		DrawAABB();

	for (uint i = 0; i < components.size(); ++i) {
		if (components[i]->IsActive())
			components[i]->Update();
	}
}

// Load & Save 
void GameObject::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file) {
	UUID = scene_file["Game Objects"][gameObjectNum]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Parent UUID"];
	json js = scene_file["Game Objects"][gameObjectNum]["Name"];
	name = js.get<std::string>();

	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);	
	
	int size = scene_file["Game Objects"][gameObjectNum]["Components"].size();	
	if (size >= 2) 
		CreateComponent(COMPONENT_TYPE::MESH);	
	if (size >= 3)
		CreateComponent(COMPONENT_TYPE::MATERIAL);
	if (size == 4)
		App->physics->AddRigidBody(obb, this, 10.0f);

	for (uint i = 0; i < components.size(); ++i)
		components[i]->OnLoad(gameObjectNum, scene_file);
}

void GameObject::OnSave(const char* gameObjectNum, nlohmann::json &scene_file) {
	scene_file["Game Objects"][gameObjectNum]["Name"] = name;
	scene_file["Game Objects"][gameObjectNum]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Parent UUID"] = parent_UUID;

	for (uint i = 0; i < components.size(); ++i)
		components[i]->OnSave(gameObjectNum, scene_file);
}

// Creates components for its GameObject
Component* GameObject::CreateComponent(COMPONENT_TYPE type, bool active) {

	Component* ret = nullptr;

	switch (type)
	{
	case COMPONENT_TYPE::MATERIAL:
		ret = new ComponentMaterial(this, active);
		if (ret != nullptr) components.push_back(ret);
		break;

	case COMPONENT_TYPE::MESH:
		ret = new ComponentMesh(this, active);
		if (ret != nullptr) components.push_back(ret); 
		App->renderer3D->meshes.push_back((ComponentMesh*)ret);
		break;

	case COMPONENT_TYPE::RIGIDBODY:
		ret = new ComponentRigidBody(this, active);
		if (ret != nullptr) components.push_back(ret);
		break;

	case COMPONENT_TYPE::TRANSFORM:
		ret = new ComponentTransform(this, active);
		if (ret != nullptr) components.push_back(ret); 
		break;

	case COMPONENT_TYPE::CAMERA:
		ret = new ComponentCamera(this, active);
		if (ret != nullptr) components.push_back(ret); 
		break;
	}

	return ret;
}

void GameObject::OnEditor() {
	for (uint i = 0; i < components.size(); ++i)
		components[i]->OnEditor();
}

void GameObject::MakeChild(GameObject* parent) {
	this->parent = parent;
	parent->children.push_back(this);
	parent_UUID = parent->GetUUID();

	//TEST
	parent->aabb.Enclose(this->aabb);
}

// Cleans the memory of the GameObject
void GameObject::CleanUp() {

	for (uint i = 0; i < components.size(); ++i) {		
		components[i]->CleanUp();
		RELEASE(components[i]);
	}
	components.clear();	
	children.clear();
	parent = nullptr;

	std::vector<GameObject*>::iterator obj_it = std::find(App->scene->nonStaticObjects.begin(), App->scene->nonStaticObjects.end(), this);
	App->scene->nonStaticObjects.erase(obj_it);
}

Component* GameObject::GetComponent(COMPONENT_TYPE componentType)
{
	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == componentType)
			return components[i];
	}

	return nullptr;
}

const Component* GameObject::GetComponent(COMPONENT_TYPE componentType) const
{
	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->type == componentType)
		{
			return *it;
		}
	}

	return nullptr;

}

void GameObject::DrawAABB()
{
	//glLineWidth(2.0f);

	glBegin(GL_LINES);

	if (App->scene->selected == this) {
		glLineWidth(4.0f);
		glColor3f(0.7, 1, 0.7); //Light green color
	}

	else {
		glLineWidth(2.0f);
		glColor3f(0.2, 0.2, 1); //Light green color
	}

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glEnd();

	glColor3f(1, 1, 1); //Blue color
}

void GameObject::UpdateAABB()
{
	ComponentMesh* mesh = GetComponent<ComponentMesh>();
	if (mesh != nullptr)
	{
		obb = mesh->GetAABB();
		obb.Transform(transform->GetGlobalTransform());

		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
}

void GameObject::MakeStatic(bool isStatic)
{
	if (isStatic)
	{
		
		for (int i = 0; i < App->scene->nonStaticObjects.size(); ++i)
		{
			if (App->scene->nonStaticObjects[i] == this)
			{
				App->scene->nonStaticObjects.erase(App->scene->nonStaticObjects.begin()+i);
			}
		}

		App->scene->sceneTree->Insert(this);
	}
	else
	{
		App->scene->sceneTree->Erase(this);
		App->scene->nonStaticObjects.push_back(this);
	}
}
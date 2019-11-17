#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleScene.h"

#include "OpenGL.h"
#include "glmath.h"
#include "mmgr/mmgr.h"

GameObject::GameObject() {
	name = "Untitled";
	MakeChild(App->scene->GetRoot());
	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM, true);
	UUID = (uint)App->GetRandomGenerator().Int();
}

GameObject::GameObject(std::string name, GameObject* parent) :	name(name)
{
	if (parent) MakeChild(parent);
	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM, true);
	
	if (strcmp("World", name.c_str()) != 0)
		UUID = (uint)App->GetRandomGenerator().Int();
}

// Called every frame
void GameObject::Update() {
	DrawAABB();

	for (uint i = 0; i < components.size(); ++i)
		if (components[i]->IsActive()) components[i]->Update();
}

// Load & Save 
void GameObject::OnLoad(const char* scene_name, const nlohmann::json &scene_file) {
	
	UUID = scene_file[scene_name]["Game Objects"]["UUID"];
	parent_UUID = scene_file[scene_name]["Game Objects"]["Parent_UUID"];

	for (uint i = 0; i < components.size(); ++i)
		components[i]->OnLoad(scene_name, scene_file);
}

void GameObject::OnSave(const char* scene_name, nlohmann::json &scene_file) {
	scene_file[scene_name]["Game Objects"][name]["UUID"] = UUID;
	scene_file[scene_name]["Game Objects"][name]["Parent UUID"] = parent_UUID;
	scene_file[scene_name]["Game Objects"][name]["Name"] = name;

	for (uint i = 0; i < components.size(); ++i)
		components[i]->OnSave(scene_name, scene_file);
}

// Creates components for its GameObject
Component* GameObject::CreateComponent(COMPONENT_TYPE type, bool active) {

	Component* ret = nullptr;

	switch (type)
	{
	case COMPONENT_TYPE::MATERIAL:
		ret = new ComponentMaterial(this, active);
		if (ret != nullptr) components.push_back(ret); break;

	case COMPONENT_TYPE::MESH:
		ret = new ComponentMesh(this, active);
		if (ret != nullptr) components.push_back(ret); break;

	case COMPONENT_TYPE::TRANSFORM:
		ret = new ComponentTransform(this, active);
		if (ret != nullptr) components.push_back(ret); break;
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
	for (uint i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == componentType)
			return components[i];
	}

	return nullptr;
}

void GameObject::DrawAABB()
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glColor3f(0.5, 1, 0.5); //Light green color

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
	if (mesh)
	{
		obb = mesh->GetAABB();
		obb.Transform(transform->GetGlobalTransform());

		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
}
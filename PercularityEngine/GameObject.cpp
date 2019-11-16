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
	this->MakeChild(App->scene->GetRoot());
	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);
	UUID = (uint)App->GetRandomGenerator().Int();
	parent_UUID = parent->GetUUID();
}

GameObject::GameObject(std::string name, GameObject* parent) :
	name(name), parent(parent)
{
	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);
	if (strcmp("World", name.c_str()) != 0)
		UUID = (uint)App->GetRandomGenerator().Int();

	if (parent)	parent_UUID = parent->GetUUID();
}

// Called every frame
void GameObject::Update() {
	for (int i = 0; i < components.size(); ++i)
		if (components[i]->active) components[i]->Update();
}

// Load & Save 
void GameObject::OnLoad(const char* scene_name, const nlohmann::json &scene_file) {
	
	UUID = scene_file[scene_name]["Game Objects"]["UUID"];
	parent_UUID = scene_file[scene_name]["Game Objects"]["Parent_UUID"];

	for (int i = 0; i < components.size(); ++i)
		components[i]->OnLoad(scene_name, scene_file);
}

void GameObject::OnSave(const char* scene_name, nlohmann::json &scene_file) {
	scene_file[scene_name]["Game Objects"]["UUID"] = UUID;
	scene_file[scene_name]["Game Objects"]["Parent UUID"] = parent_UUID;
	scene_file[scene_name]["Game Objects"]["Name"] = name;

	for (int i = 0; i < components.size(); ++i)
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
	for (int i = 0; i < components.size(); ++i)
		components[i]->OnEditor();
}

void GameObject::MakeChild(GameObject* parent) {
	this->parent = parent;
	parent->children.push_back(this);
}

// Cleans the memory of the GameObject
void GameObject::CleanUp() {

	for (int i = 0; i < components.size(); ++i) {		
		components[i]->CleanUp();
		RELEASE(components[i]);
	}
}

Component* GameObject::GetComponent(COMPONENT_TYPE componentType)
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == componentType)
			return components[i];
	}

	return nullptr;
}

const Component* GameObject::GetComponent(COMPONENT_TYPE componentType) const
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == componentType)
			return components[i];
	}

	return nullptr;
}
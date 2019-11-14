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

GameObject::GameObject():name("Untitled"), parent(nullptr){

	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::GameObject(std::string name, GameObject* parent) :
	name(name), parent(parent)
{
	transform = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);
	/*if (!strcmp(name.c_str(), "World"))	
		UUID = App->scene->GenerateRandomUUID();*/
}

// Called every frame
void GameObject::Update() {
	for (int i = 0; i < components.size(); ++i)
		if (components[i]->active) components[i]->Update();
}

// Creates components for its GameObject
Component* GameObject::CreateComponent(COMPONENT_TYPE type, bool active) {

	Component* ret = nullptr;

	switch (type)
	{
	case COMPONENT_TYPE::MATERIAL:
		ret = new ComponentMaterial(type, this, active);
		if (ret != nullptr) components.push_back(ret); break;

	case COMPONENT_TYPE::MESH:
		ret = new ComponentMesh(type, this, active);
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

//Save & Load
void GameObject::OnLoad(const nlohmann::json &config){
	for (int i = 0; i < components.size(); ++i)
		components[i]->OnLoad(config);
}

void GameObject::OnSave(nlohmann::json &config) {
	for (int i = 0; i < components.size(); ++i)
		components[i]->OnSave(config);
}

// Cleans the memory of the GameObject
void GameObject::CleanUp() {

	for (int i = 0; i < components.size(); ++i) {
		//c_mesh->CleanUp();
		components[i]->CleanUp();
		delete components[i];
		components[i] = nullptr;
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
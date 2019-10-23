#ifndef __GameObject_H__
#define __GameObject_H__

#include "Component.h"
#include <vector>

#define NORMALS_LENGTH 1.0f

class ComponentMesh;
class ComponentMaterial;
class ComponentTransform;

class GameObject {
public:
	//Constructors
	GameObject() {};
	GameObject(char* name, GameObject* parent = nullptr);

	// Destructor
	virtual ~GameObject() {};

	// Rendering methods
	void Render() const;
	void ShowNormals();

	// Called every frame
	void Update();

	// Removes the memory
	void CleanUp();

	Component* CreateComponent(COMPONENT_TYPE type, bool active = true);
	void OnEditor();

public:
	ComponentMesh* c_mesh = nullptr;
	ComponentMaterial* c_texture = nullptr;
	ComponentTransform* c_transform;

	char* name;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent;
};

#endif // __GameObject_H__
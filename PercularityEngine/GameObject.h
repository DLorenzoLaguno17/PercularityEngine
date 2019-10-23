#ifndef __GameObject_H__
#define __GameObject_H__

#include "Component.h"
//#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>

#define NORMALS_LENGTH 1.0f

class ComponentMesh;
class ComponentMaterial;

class GameObject {
public:
	//Constructors
	GameObject() {};
	GameObject(char* name, GameObject* parent = nullptr);

	// Destructor
	virtual ~GameObject() {};

	void Start();

	// Rendering methods
	void Render() const;
	void ShowNormals();

	// Called every frame
	void Update();

	// Removes the memory
	void CleanUp();

	Component* CreateComponent(COMPONENT_TYPE type, bool active = true);

public:
	// List of data
	ComponentMesh* c_mesh = nullptr;
	// Texture of the GameObject
	ComponentMaterial* c_texture = nullptr;
	// Transform
	// ComponentTransform* transform;

	char* name;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent;
};

#endif // __GameObject_H__
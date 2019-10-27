#ifndef __GameObject_H__
#define __GameObject_H__

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"
#include <vector>
#include <string>

class ComponentMesh;
class ComponentMaterial;
class ComponentTransform;

struct BoundingBox {
	BoundingBox() {
		//Initialize the bounding box as this, in case there's no mesh
		for (int i = 0; i < 8; ++i)
			box[i] = { 0,0,0 };
	}

	float3 box[8];
	float minX, maxX, minY, maxY, minZ, maxZ = 0.0f;
};

class GameObject {
public:
	//Constructors
	GameObject();
	GameObject(std::string name, GameObject* parent = nullptr);

	// Destructor
	virtual ~GameObject() {};

	// Called every frame
	void Update();

	// Removes the memory
	void CleanUp();

	Component* CreateComponent(COMPONENT_TYPE type, bool active = true);
	void OnEditor();

public:
	ComponentMesh* c_mesh = nullptr;
	ComponentMaterial* c_texture = nullptr;
	ComponentTransform* c_transform = nullptr;

	std::string name;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent;

	BoundingBox boundingBox;
};

#endif // __GameObject_H__
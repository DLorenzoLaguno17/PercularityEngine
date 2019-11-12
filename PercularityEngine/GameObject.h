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

	// Makes a GameObject its parent
	void MakeParent(GameObject* parent);

	Component* CreateComponent(COMPONENT_TYPE type, bool active = true);
	void OnEditor();

	Component* GetComponent(COMPONENT_TYPE componentType);
	const Component* GetComponent(COMPONENT_TYPE componentType) const;

	template<class T>
	T* GetComponent() { return reinterpret_cast<T*>(GetComponent(T::GetComponentType())); }
	
public:
	ComponentTransform* transform = nullptr;

	std::string name;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent;
	uint UUID = 0;

	BoundingBox boundingBox;
	bool extended = true;
};

#endif // __GameObject_H__
#ifndef __GameObject_H__
#define __GameObject_H__

#include "MathGeoLib/include/MathGeoLib.h"
#include "Component.h"
#include "Globals.h"
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

	void Update();
	void CleanUp();

	// Load & Save 
	void OnLoad(const char* scene_name, const nlohmann::json &scene_file);
	void OnSave(const char* scene_name, nlohmann::json &scene_file);

	// Makes a it child of another GameObject
	void MakeChild(GameObject* parent);

	uint GetUUID() const { return UUID; }

	Component* CreateComponent(COMPONENT_TYPE type, bool active = true);
	void OnEditor();

	Component* GetComponent(COMPONENT_TYPE componentType);
	const Component* GetComponent(COMPONENT_TYPE componentType) const;

	template<class T>
	T* GetComponent() { return reinterpret_cast<T*>(GetComponent(T::GetComponentType())); }
	
public:
	ComponentTransform* transform = nullptr;

	std::string name;
	std::vector<GameObject*> children;
	GameObject* parent;

	BoundingBox boundingBox;
	bool extended = true;

private:
	std::vector<Component*> components;
	uint UUID = 0;
	uint parent_UUID = 0;

};

#endif // __GameObject_H__
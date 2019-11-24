#ifndef __GameObject_H__
#define __GameObject_H__

#include "Component.h"

class ComponentMesh;
class ComponentMaterial;
class ComponentTransform;

class GameObject {
public:
	//Constructors
	GameObject();
	GameObject(std::string name, GameObject* parent = nullptr, bool loadingScene = false);

	// Destructor
	virtual ~GameObject() {};

	void Update();
	void CleanUp();

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);

	// Makes a it child of another GameObject
	void MakeChild(GameObject* parent);
	void UpdateAABB();
	void DrawAABB();

	uint GetUUID() const { return UUID; }

	Component* CreateComponent(COMPONENT_TYPE type, bool active = true);
	void OnEditor();

	Component* GetComponent(COMPONENT_TYPE componentType);
	const Component* GetComponent(COMPONENT_TYPE componentType) const;

	template<class T>
	T* GetComponent() { return reinterpret_cast<T*>(GetComponent(T::GetComponentType())); }

	template<class T>
	const T* GetComponent()const  { return reinterpret_cast<const T*>(GetComponent(T::GetComponentType())); }
	
public:
	std::vector<Component*> components;
	ComponentTransform* transform = nullptr;

	std::string name;
	std::vector<GameObject*> children;
	GameObject* parent;

	bool extended = true;
	bool showBondingBox = true;

	AABB aabb;
	OBB obb;

	bool isStatic = false;

private:
	uint UUID = 0;
	uint parent_UUID = 0;
};

#endif // __GameObject_H__
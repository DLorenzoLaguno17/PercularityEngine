#ifndef __Component_H__
#define __Component_H__

#include "Globals.h"

class GameObject;

enum class COMPONENT_TYPE {
	NONE,
	TRANSFORM,
	MATERIAL,
	MESH
};

class Component {
public:
	//Constructors
	Component(COMPONENT_TYPE type, GameObject* parent = nullptr, bool active = true) :
		type(type), active(active), parent(parent) {}

	// Destructor
	virtual ~Component() {}

	// Methods
	virtual void Update() {}
	virtual void OnEditor() {}
	virtual void CleanUp() {}

	// Load & Save 
	virtual void OnLoad(const char* scene_name, const nlohmann::json &scene_file) {}
	virtual void OnSave(const char* scene_name, nlohmann::json &scene_file) {}

	uint GetUUID() const { return UUID; }
	COMPONENT_TYPE GetType() const { return type; }

public:
	COMPONENT_TYPE type;
	GameObject* parent;
	bool active;

protected:
	uint UUID = 0;
	uint parent_UUID = 0;
};

#endif // __Component_H__
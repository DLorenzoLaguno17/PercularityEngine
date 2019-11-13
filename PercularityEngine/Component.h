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
	virtual void OnLoad(const nlohmann::json &config) {}
	virtual void OnSave(nlohmann::json &config) {}

	COMPONENT_TYPE GetType() const { return type; }

public:
	COMPONENT_TYPE type;
	GameObject* parent;
	bool active;
};

#endif // __Component_H__
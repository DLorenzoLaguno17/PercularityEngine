#ifndef __Component_H__
#define __Component_H__

#include "Globals.h"

class GameObject;

enum class COMPONENT_TYPE {
	NONE,
	TRANSFORM,
	TEXTURE,
	MESH
};

class Component {
public:
	//Constructors
	Component(COMPONENT_TYPE type, bool active, GameObject* parent = nullptr) : 
		type(type), active(active), parent(parent) {}

	// Destructor
	virtual ~Component() {}

	// Methods
	virtual void Update() {}
	virtual void Enable() {}
	virtual void Disable() {}

public:
	COMPONENT_TYPE type;
	GameObject* parent;
	bool active;
};

#endif // __Component_H__
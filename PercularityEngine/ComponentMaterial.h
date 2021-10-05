#ifndef _ComponentMaterial_H_
#define _ComponentMaterial_H_

#include "Component.h"
#include "ModuleUndo.h"

class ResourceTexture;

class ComponentMaterial : public Component 
{
public:
	ComponentMaterial(GameObject* parent, bool active, ComponentMaterial* reference);

	// Destructor
	~ComponentMaterial() {}

	// Methods
	void Update() {}
	void OnEditor();
	void CleanUp();

	// Load & Save 
	void OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file);
	void OnSave(const char* gameObjectNum, nlohmann::json &scene_file);

	static COMPONENT_TYPE GetComponentType() { return COMPONENT_TYPE::MATERIAL; }

public:
	ResourceTexture* resource_tex = nullptr;
};

// ---------------------------------------------------
class ChangeTexture : public Action
{
public:
	ChangeTexture(uint lastTexture, uint newTexture, uint gameObject_uuid) :
		lastTexture_uuid(lastTexture), newTexture_uuid(newTexture), uuid(gameObject_uuid) {}

	void Undo() override;
	void Redo() override;

	uint lastTexture_uuid;
	uint newTexture_uuid;
	uint uuid;
};

#endif // _ComponentMaterial_H_
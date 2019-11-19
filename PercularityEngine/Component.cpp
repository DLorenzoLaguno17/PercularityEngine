#include "Component.h"
#include "GameObject.h"
#include "Application.h"

Component::Component(COMPONENT_TYPE type, GameObject* parent, bool active) :
	type(type), active(active), gameObject(parent)
{
	UUID = (uint)App->GetRandomGenerator().Int();
	if (parent) parent_UUID = parent->GetUUID();
}
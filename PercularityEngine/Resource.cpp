#include "Resource.h"

Resource::Resource(UID uid, RESOURCE_TYPE type) : 
	uid(uid), type(type) {}

bool Resource::LoadToMemory() {

	return true;
}
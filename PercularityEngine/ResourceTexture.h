#ifndef __ResourceTexture_H__
#define __ResourceTexture_H__

#include "Globals.h"
#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(uint uuid) : Resource(uuid, RESOURCE_TYPE::TEXTURE) {}

	virtual ~ResourceTexture() {}

	bool LoadInMemory() { return true; }
	void ReleaseFromMemory() {}

	void Save(nlohmann::json &config) const {}
	void Load(const nlohmann::json &config) {}

public:
	uint texture = 0;
	long width = 0;
	long height = 0;
};

#endif // __ResourceTexture_H__
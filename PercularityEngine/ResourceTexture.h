#ifndef __ResourceTexture_H__
#define __ResourceTexture_H__

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(uint uuid) : Resource(uuid, RESOURCE_TYPE::TEXTURE) {}

	virtual ~ResourceTexture() {}

	bool LoadInMemory();
	void ReleaseFromMemory();

	void OnSave(const char* resourceNum, json &config) const;
	void OnLoad(const char* resourceNum, const json &config);

public:
	uint texture = 0;
	uint width = 0;
	uint height = 0;
};

#endif // __ResourceTexture_H__
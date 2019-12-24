#ifndef __ResourceScene_H__
#define __ResourceScene_H__

#include "PercularityResource.h"

class ResourceScene : public Resource
{
public:
	ResourceScene(uint uuid) : Resource(uuid, RESOURCE_TYPE::SCENE) {}

	virtual ~ResourceScene() {}

	bool LoadInMemory();
	void ReleaseFromMemory();

	void OnSave(const char* resourceNum, json &config) const;
	void OnLoad(const char* resourceNum, const json &config);

public:
};

#endif // __ResourceScene_H__
#ifndef __ResourceScene_H__
#define __ResourceScene_H__

#include "Resource.h"

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
	json scene_file;
};

#endif // __ResourceScene_H__
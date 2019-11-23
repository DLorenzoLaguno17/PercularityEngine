#ifndef __ResourceModel_H__
#define __ResourceModel_H__

#include "Resource.h"

class ResourceMesh;

class ResourceModel : public Resource
{
public:
	ResourceModel(uint uuid) : Resource(uuid, RESOURCE_TYPE::MODEL) {}

	virtual ~ResourceModel() {}

	bool LoadInMemory();
	void ReleaseFromMemory();

	void OnSave(const char* resourceNum, json &config) const;
	void OnLoad(const char* resourceNum, const json &config);

public:
	std::vector<ResourceMesh*> meshes;

};

#endif // __ResourceTexture_H__
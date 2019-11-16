#ifndef __ResourceTexture_H__
#define __ResourceTexture_H__

#include "Globals.h"
#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(UID id);
	virtual ~ResourceTexture();
	bool LoadInMemory() override;
	void Save(nlohmann::json &config) const override;
	void Load(const nlohmann::json &config) override;

public:
	uint width = 0;
	uint height = 0;
	uint depth = 0;
	uint mips = 0;
	uint bytes = 0;
	uint gpu_id = 0;
	//Format format = unknown;
};

#endif // __ResourceTexture_H__
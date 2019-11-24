#ifndef __Resource_H__
#define __Resource_H__

#include "Globals.h"

enum class RESOURCE_TYPE {
	TEXTURE,
	MESH,
	MODEL,
	SCENE,
	UNKNOWN
};

class Resource
{
public:
	Resource(uint uuid, RESOURCE_TYPE type) : UUID(uuid), type(type) {}

	virtual ~Resource() {}

	// Getters
	uint GetUUID() const { return UUID; }
	void SetUUID( uint uuid) { UUID = uuid; }
	const char* GetFile() const { return file.c_str(); }
	const char* GetExportedFile() const { return exported_file.c_str(); }

	virtual void OnLoad(const char* resourceNum, const json &config) {}
	virtual void OnSave(const char* resourceNum, json &config) const {}

	bool IsLoadedToMemory() const { return usedAsReference > 0; }
	bool UpdateReferenceCount() 
	{
		if (usedAsReference > 0) 
			usedAsReference++;
		else if (LoadInMemory()) 
			usedAsReference = 1;
		else 
			usedAsReference = 0;

		return usedAsReference > 0;
	}

	virtual void ReleaseFromMemory() = 0;
protected:
	virtual bool LoadInMemory() = 0;

public:
	// Number of times this resurce is used
	uint usedAsReference = 0;
	RESOURCE_TYPE type = RESOURCE_TYPE::UNKNOWN;
	std::string file = "";
	std::string exported_file = "";
	std::string name = "";

protected:
	uint UUID = 0;
};

#endif // __Resource_H__
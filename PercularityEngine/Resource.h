#ifndef __Resource_H__
#define __Resource_H__

#include "Globals.h"

enum class RESOURCE_TYPE {
	TEXTURE,
	MESH,
	SCENE,
	UNKNOWN
};

class Resource
{
public:
	Resource(UID uid, RESOURCE_TYPE type);

	virtual ~Resource() {}

	// Getters
	UID GetUID() const { return uid; }
	const char* GetFile() const { return file.c_str(); }
	const char* GetImportedFile() const { return imported_file.c_str(); }

	bool IsLoadedToMemory() const { return usedAsReference > 0; } 
	bool LoadToMemory();

	virtual void Load(const nlohmann::json &config) {}
	virtual void Save(nlohmann::json &config) const {}

protected:
	virtual bool LoadInMemory() = 0;
	virtual void ReleaseFromMemory() = 0;

public:
	// Times this resurce is used
	uint usedAsReference = 0;
	RESOURCE_TYPE type = RESOURCE_TYPE::UNKNOWN;

protected:
	UID uid = 0;
	std::string file = "";
	std::string imported_file = "";
	std::string name = "";
};

#endif // __Resource_H__
#ifndef __Resource_H__
#define __Resource_H__

#include "Globals.h"

enum RESOURCE_TYPE {
	TEXTURE,
	MESH,
	AUDIO,
	SCENE,
	UNKNOWN
};

class Resource
{
public:
	Resource(UID uid, RESOURCE_TYPE type) : uid(uid), type(type){}
	virtual ~Resource() {}
	RESOURCE_TYPE GetType() const;
	UID GetUID() const;
	const char* GetFile() const;
	const char* GetImportedFile() const;
	bool IsLoadedToMemory() const;
	bool LoadToMemory();
	uint CountReferences() const;

	virtual void Save(Config& config) const;
	virtual void Load(const Config& config);
	virtual bool LoadInMemory() = 0;

protected:
	UID uid = 0;
	std::string file;
	std::string imported_file;
	RESOURCE_TYPE type = RESOURCE_TYPE::UNKNOWN;
	uint loaded = 0;
};

#endif // __Resources_H__
#ifndef _MODULE_SCENE_H__
#define _MODULE_SCENE_H__

#include "Module.h"
#include "Primitive.h"

class ModuleScene : public Module
{
public:

	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	
	//Save & Load
	void Load(const nlohmann::json &config) {}
	void Save(nlohmann::json &config) {}

private:

	//TEST SHAPE
	Cone* lmao;

	void DrawAxis() const;		//Draw XYZ axis of coordinates
	void DrawSimplePlane()const;//Draw a plane with some lines
};

#endif
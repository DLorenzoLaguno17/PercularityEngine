#ifndef _MODULE_SCENE_H__
#define _MODULE_SCENE_H__

#include "Module.h"

class GameObject;

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

	//Methods to create primitives
	GameObject* CreateSphere(int slices, int stacks, float diameter);
	GameObject* CreateCube(float sizeX, float sizeY, float sizeZ);
	GameObject* CreatePlane(float length, float depth);
	GameObject* CreateDonut(int slices, int stacks, float radius);

private:
	void DrawAxis() const;			//Draw XYZ axis of coordinates
	void DrawSimplePlane()const;	//Draw a plane with some lines

public:
	std::vector<GameObject*> game_objects;
	GameObject* root = nullptr;

	GameObject* selected = nullptr;
	int selected_id = -1;

};
#endif
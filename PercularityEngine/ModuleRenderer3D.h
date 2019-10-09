#pragma once

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:
	void OnResize(int width, int height);

	void DrawAxis() const;		//Draw XYZ axis of coordinates
	void DrawSimplePlane()const;//Draw a plane with some lines

	//Delete
	void DrawDirectCube()const;	//Draw a cube in direct mode = Hardcoded

private:

	//Testing vertex arrays
	void CreateCube();			

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	update_status status = UPDATE_CONTINUE;
};
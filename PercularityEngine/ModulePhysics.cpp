#include "ModulePhysics.h"
#include "Globals.h"
#include "stdio.h"

#ifdef _DEBUG
	#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
	#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init()
{

	
	return true;
}

bool ModulePhysics::Start()
{

	return true;
}

update_status ModulePhysics::PreUpdate(float dt)
{
	world->stepSimulation(dt,60);

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	return true;
}
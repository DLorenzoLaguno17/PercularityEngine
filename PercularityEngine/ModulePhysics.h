#ifndef _MODULE_PHYSICS_H__
#define _MODULE_PHYSICS_H__

#include "Module.h"

#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

class ModulePhysics :public Module
{
public:

	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Init();
	bool Start(); 
	
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

private:

	btDefaultCollisionConfiguration*		collision_conf=nullptr;
	btCollisionDispatcher*					dispatcher = nullptr;
	btBroadphaseInterface*					broad_phase = nullptr;
	btSequentialImpulseConstraintSolver*	solver = nullptr;
	btDiscreteDynamicsWorld*				world = nullptr;
	btDefaultVehicleRaycaster*				vehicle_raycaster = nullptr;

};

#endif

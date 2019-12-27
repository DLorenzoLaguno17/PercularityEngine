#ifndef _MODULE_PHYSICS_H__
#define _MODULE_PHYSICS_H__

#include "Module.h"
#include "glmath.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

struct PhysVehicle;
struct VehicleInfo;
class ComponentRigidbody;
class DebugDrawer;

class ModulePhysics :public Module
{
public:

	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Init();
	bool Start(); 
	bool CleanUp();
	
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void AddRigidBody();
	PhysVehicle* AddVehicle(const VehicleInfo &info);

	void AddConstraintP2P(ComponentRigidbody& bodyA, ComponentRigidbody& bodyB, const vec3& anchorA, const vec3& anchorB);
	void AddConstraintHinge(ComponentRigidbody& bodyA, ComponentRigidbody& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisS, const vec3& axisB, bool disable_collision = false);

public:
	std::vector<btCollisionShape*> shapes;
	std::vector<btDefaultMotionState*> motions;
	std::vector<PhysVehicle*> vehicles;
	std::vector<btTypedConstraint*> constraints;

private:
	btDefaultCollisionConfiguration*		collision_conf = nullptr;
	btCollisionDispatcher*					dispatcher = nullptr;
	btBroadphaseInterface*					broad_phase = nullptr;
	btSequentialImpulseConstraintSolver*	solver = nullptr;
	btDiscreteDynamicsWorld*				world = nullptr;
	btDefaultVehicleRaycaster*				vehicle_raycaster = nullptr;
	DebugDrawer*							debugDrawer = nullptr;
	
};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() {}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	Line line;
	//Primitive point;
};
#endif

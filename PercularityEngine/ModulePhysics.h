#ifndef _MODULE_PHYSICS_H__
#define _MODULE_PHYSICS_H__

#include "Module.h"
#include "glmath.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 
#define MASS 0

struct PhysVehicle;
struct VehicleInfo;
class ComponentRigidbody;

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

	ComponentRigidbody* AddBody(const PrimitiveCube& cube, float mass);
	PhysVehicle* AddVehicle(const VehicleInfo &info);
	void CreateTestConstraint();
	void DeleteTestConstraint();
	void ShootBall();
	void ClearBalls();

	void AddConstraintP2P(ComponentRigidbody& bodyA, ComponentRigidbody& bodyB, const vec3& anchorA, const vec3& anchorB);
	void AddConstraintHinge(ComponentRigidbody& bodyA, ComponentRigidbody& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisS, const vec3& axisB, bool disable_collision = false);

public:
	std::vector<btCollisionShape*> shapes;
	std::vector<btDefaultMotionState*> motions;
	std::vector<PhysVehicle*> vehicles;
	std::vector<btTypedConstraint*> constraints;
	std::vector<ComponentRigidbody*> bodies;

	std::vector<ComponentRigidbody*> balls;
	std::vector<PrimitiveSphere*> spheres;
	std::vector<btRigidBody*> physBalls;

private:
	btDefaultCollisionConfiguration*		collision_conf = nullptr;
	btCollisionDispatcher*					dispatcher = nullptr;
	btBroadphaseInterface*					broad_phase = nullptr;
	btSequentialImpulseConstraintSolver*	solver = nullptr;
	btDiscreteDynamicsWorld*				world = nullptr;
	btDefaultVehicleRaycaster*				vehicle_raycaster = nullptr;

	// Elements for the test constraints
	PrimitiveCube* right_cube = nullptr;
	PrimitiveCube* left_cube = nullptr;
	ComponentRigidbody* right_body = nullptr;
	ComponentRigidbody* left_body = nullptr;
};

#endif

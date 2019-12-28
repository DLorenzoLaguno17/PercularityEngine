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
class ComponentRigidBody;
class ComponentMesh;
class GameObject;

enum class SHAPE_TYPE {
	none=-1,
	BOX,
	CAPSULE,
	SPHERE,
	CONVEX_HULL
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

	ComponentRigidBody* AddPrimitiveBody(const PrimitiveCube& cube, float mass);
	ComponentRigidBody* AddRigidBody(OBB& box,  GameObject* gameObject,float mass = 1.0f );
	ComponentRigidBody* AddRigidBody(Sphere& sphere, GameObject* gameObject, float mass = 1.0f);
	ComponentRigidBody* AddRigidBody(Capsule& capsule, GameObject* gameObject, float mass = 1.0f);

	PhysVehicle* AddVehicle(const VehicleInfo &info);
	void CreateTestConstraint();
	void DeleteTestConstraint();
	void ShootBall();
	void ClearBalls();

	void AddConstraintP2P(ComponentRigidBody& bodyA, ComponentRigidBody& bodyB, const vec3& anchorA, const vec3& anchorB);
	void AddConstraintHinge(ComponentRigidBody& bodyA, ComponentRigidBody& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisS, const vec3& axisB, bool disable_collision = false);

public:
	std::vector<btCollisionShape*> shapes;
	std::vector<btDefaultMotionState*> motions;
	std::vector<PhysVehicle*> vehicles;
	std::vector<btTypedConstraint*> constraints;
	std::vector<ComponentRigidBody*> rigidBodies;

	std::vector<ComponentRigidBody*> balls;
	std::vector<PrimitiveSphere*> spheres;
	std::vector<btRigidBody*> physBalls;

private:
	btDefaultCollisionConfiguration*		collision_conf = nullptr;
	btCollisionDispatcher*					dispatcher = nullptr;
	btBroadphaseInterface*					broad_phase = nullptr;
	btSequentialImpulseConstraintSolver*	solver = nullptr;
	btDiscreteDynamicsWorld*				world = nullptr;
	btDefaultVehicleRaycaster*				vehicle_raycaster = nullptr;
	DebugDrawer*							debugDrawer = nullptr;
	
	ComponentRigidBody* testdude = nullptr;

	// Elements for the test constraints
	PrimitiveCube* right_cube = nullptr;
	PrimitiveCube* left_cube = nullptr;
	ComponentRigidBody* right_body = nullptr;
	ComponentRigidBody* left_body = nullptr;
};

#endif
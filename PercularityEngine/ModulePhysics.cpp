#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentRigidbody.h"
#include "ComponentTransform.h"
#include "PhysVehicle.h"
#include "Globals.h"
#include "Primitive.h"
#include "stdio.h"

#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

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
	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
}

ModulePhysics::~ModulePhysics() {}

bool ModulePhysics::Init() 
{
	LOG("Initializing physics module");
	return true;
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics environment");
	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	//world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	// Big plane as ground
	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

	btDefaultMotionState* myMotionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);
	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);

	// Creating cubes for the test constraint
	right_cube = new PrimitiveCube(3, 4, 3);
	left_cube = new PrimitiveCube(3, 4, 3);	
	right_cube->SetPos(0, 2, 25);
	left_cube->SetPos(0, 2, 25);
	right_cube->color.Set(Red.r, Red.g, Red.b);
	left_cube->color.Set(Green.r, Green.g, Green.b);
	right_body = App->physics->AddBody(*right_cube, 15.0f);
	left_body = App->physics->AddBody(*right_cube, 15.0f);

	return true;
}

update_status ModulePhysics::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PhysicsPreUpdate", Profiler::Color::Orange);

	world->stepSimulation(dt, 15);
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::Update(float dt)
{
	BROFILER_CATEGORY("PhysicsUpdate", Profiler::Color::LightSeaGreen);
	
	right_body->GetTransform(&right_cube->transform);
	left_body->GetTransform(&left_cube->transform);
	right_cube->Render();
	left_cube->Render();

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PhysicsPostUpdate", Profiler::Color::Yellow);

	return UPDATE_CONTINUE;
}

ComponentRigidbody* ModulePhysics::AddBody(const PrimitiveCube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x * 0.5f, cube.size.y * 0.5f, cube.size.z * 0.5f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	ComponentRigidbody* pbody = new ComponentRigidbody(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}

bool ModulePhysics::CleanUp()
{
	LOG("Destroying Physics module");

	// Remove from the world all collision bodies
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	// Clear all the lists
	for (int i = 0; i < constraints.size(); ++i)
	{
		world->removeConstraint(constraints[i]);
		RELEASE(constraints[i]);
	}

	constraints.clear();

	for (int i = 0; i < motions.size(); ++i)
		RELEASE(motions[i])

	motions.clear();

	for (int i = 0; i < shapes.size(); ++i)
		RELEASE(shapes[i])

	shapes.clear();

	for (int i = 0; i < bodies.size(); ++i)
		RELEASE(bodies[i])

	bodies.clear();

	if (vehicles.size() > 1) {
		for (int i = 0; i < vehicles.size(); ++i)
			RELEASE(vehicles[i])

		vehicles.clear();
	}

	// Delete the pointers
	RELEASE(left_cube);
	RELEASE(right_cube);
	RELEASE(vehicle_raycaster);
	RELEASE(world);
	RELEASE(solver);
	RELEASE(broad_phase);
	RELEASE(dispatcher);
	RELEASE(collision_conf);

	return true;
}

// Cretes a vehicle and adds it to the scene
PhysVehicle* ModulePhysics::AddVehicle(const VehicleInfo &info)
{
	btCompoundShape* comShape = new btCompoundShape();
	shapes.push_back(comShape);

	btCollisionShape* colShape = new btBoxShape(btVector3(info.chassis_size.x * 0.5f, info.chassis_size.y * 0.5f, info.chassis_size.z*0.5f));
	shapes.push_back(colShape);

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z));

	comShape->addChildShape(trans, colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for (int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}

	PhysVehicle* phys_vehicle = new PhysVehicle(body, vehicle, info);
	world->addVehicle(vehicle);
	vehicles.push_back(phys_vehicle);

	return phys_vehicle;
}

// Constraint creators
void ModulePhysics::AddConstraintP2P(ComponentRigidbody &bodyA, ComponentRigidbody &bodyB, const vec3 &anchorA, const vec3 &anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z));

	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void ModulePhysics::AddConstraintHinge(ComponentRigidbody &bodyA, ComponentRigidbody &bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z),
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);
}

void ModulePhysics::CreateTestConstraint()
{
	AddConstraintP2P(*right_body, *left_body, { -5, 0, 0 }, { 5, 0, 0 });
}

void ModulePhysics::DeleteTestConstraint()
{
	for (int i = 0; i < constraints.size(); ++i)
	{
		world->removeConstraint(constraints[i]);
		RELEASE(constraints[i]);
	}

	constraints.clear();
}
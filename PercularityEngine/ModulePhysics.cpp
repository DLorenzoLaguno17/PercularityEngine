#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "PhysVehicle.h"
#include "Globals.h"
#include "stdio.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "Time.h"

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
	debugDrawer = new DebugDrawer();
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
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	// Big plane as ground
	btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

	btDefaultMotionState* myMotionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);
	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);
	world->setDebugDrawer(debugDrawer);

	return true;
}

update_status ModulePhysics::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PhysicsPreUpdate", Profiler::Color::Orange);

	if (Time::running)
		world->stepSimulation(dt, 15);

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::Update(float dt)
{
	BROFILER_CATEGORY("PhysicsUpdate", Profiler::Color::LightSeaGreen);

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PhysicsPostUpdate", Profiler::Color::Yellow);
	world->debugDrawWorld();
	return UPDATE_CONTINUE;
}

void ModulePhysics::AddRigidBody() 
{
	// First we create the Component Rigidbody and the collision shape
	float mass = 30.0f;
	ComponentRigidBody* rigidbody = (ComponentRigidBody*)App->scene->selected->CreateComponent(COMPONENT_TYPE::RIGIDBODY);
	btCollisionShape* colShape = new btBoxShape(btVector3(4 * 0.5f, 4 * 0.5f, 4 * 0.5f));
	shapes.push_back(colShape);

	// The we set its transform
	ComponentTransform* transform = (ComponentTransform*)App->scene->selected->GetComponent(COMPONENT_TYPE::TRANSFORM);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&transform->renderTransform);

	// We calculate the local inertia
	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	// To finish we create the Physbody and add it to the world
	btRigidBody* body = new btRigidBody(rbInfo);
	rigidbody->CreateBody(body);
	world->addRigidBody(body);
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

	if (vehicles.size() > 1) {
		for (int i = 0; i < vehicles.size(); ++i)
			RELEASE(vehicles[i])

			vehicles.clear();
	}

	// Delete the pointers
	RELEASE(vehicle_raycaster);
	RELEASE(world);
	RELEASE(solver);
	RELEASE(broad_phase);
	RELEASE(dispatcher);
	RELEASE(collision_conf);

	return true;
}

ComponentRigidBody* ModulePhysics::AddRigidBody(OBB& box,  GameObject* gameObject, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(box.HalfSize().x, box.HalfSize().y, box.HalfSize().z));

	btTransform transform;
	transform.setFromOpenGLMatrix(&gameObject->transform->renderTransform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	ComponentRigidBody* component = new ComponentRigidBody(body);

	world->addRigidBody(body);
	
	return component;
	/*
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;*/
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
void ModulePhysics::AddConstraintP2P(ComponentRigidBody &bodyA, ComponentRigidBody &bodyB, const vec3 &anchorA, const vec3 &anchorB)
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

void ModulePhysics::AddConstraintHinge(ComponentRigidBody &bodyA, ComponentRigidBody &bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
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


//~~~~	DEBUG DRAWER	~~~~
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	glColor3f(color.x(), color.y(), color.z());

	glVertex3f(from.x(), from.y(), from.z());
	glVertex3f(to.x(), to.y(), to.z());
	
	glEnd();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	/*point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();*/
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes)debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
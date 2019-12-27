#pragma once

#include "Module.h"
#include "glmath.h"

struct PhysVehicle;

#define MAX_ACCELERATION 500.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module {
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void ResetCar();

public:
	PhysVehicle* vehicle = nullptr;
	vec3 initialCarPosition;
	vec3 initialForwardVector;

	float turn;
	float acceleration;
	float brake;

	bool goingForward = false;
	bool freeCamera = false;
	bool startedEngine = false;
	bool collided = false;
};
#include "Application.h"
#include "ModulePhysics.h"
#include "ComponentRigidbody.h"
#include "ImGui/imgui.h"

#include "mmgr/mmgr.h"

ComponentRigidbody::ComponentRigidbody(GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::RIGIDBODY, parent, active)
{}

void ComponentRigidbody::Update() {}

void ComponentRigidbody::OnEditor()
{
	if (ImGui::CollapsingHeader("RigidBody")) {
		ImGui::Checkbox("Enabled", &active);


		ImGui::NewLine();
	}
}

// Releases all the data
void ComponentRigidbody::CleanUp() 
{
	RELEASE(body);
}

void ComponentRigidbody::CreateBody(btRigidBody* body)
{
	body->setUserPointer(this);
}

// Load & Save 
void ComponentRigidbody::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file)
{
	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Active"];
}

void ComponentRigidbody::OnSave(const char* gameObjectNum, nlohmann::json &scene_file)
{
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Active"] = active;
}

// Pushes the rigidbody with velocity determined by a vector
void ComponentRigidbody::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// Gets the transform of the Rigidbody
void ComponentRigidbody::GetTransform(float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// Sets the transform of the Rigidbody
void ComponentRigidbody::SetTransform(const float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// Sets the position of the Rigidbody
void ComponentRigidbody::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}

// Rotates the Rigidbody
void ComponentRigidbody::RotateBody(btQuaternion rotationQuaternion) {

	btTransform transform = body->getWorldTransform();

	transform.setRotation(rotationQuaternion);
	body->setWorldTransform(transform);
}
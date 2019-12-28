#include "Application.h"
#include "ModulePhysics.h"
#include "ComponentRigidBody.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Time.h"

#include "mmgr/mmgr.h"

ComponentRigidBody::ComponentRigidBody(GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::RIGIDBODY, parent, active) {}

ComponentRigidBody::ComponentRigidBody(btRigidBody* body) : body(body)
{
	body->setUserPointer(this);
}

void ComponentRigidBody::Update() {
	/*if (Time::running)
	{
		body->getWorldTransform().getOpenGLMatrix(&gameObject->transform->renderTransform);
	}*/
}

void ComponentRigidBody::OnEditor()
{
	if (ImGui::CollapsingHeader("RigidBody")) {
		ImGui::Checkbox("Enabled", &active);


		ImGui::NewLine();
	}
}

// Releases all the data
void ComponentRigidBody::CleanUp() 
{
	//delete body;
}

void ComponentRigidBody::CreateBody(btRigidBody* body)
{
	body->setUserPointer(this);
}

// Load & Save 
void ComponentRigidBody::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file)
{
	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Active"];
}

void ComponentRigidBody::OnSave(const char* gameObjectNum, nlohmann::json &scene_file)
{
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Active"] = active;
}

// Pushes the rigidbody with velocity determined by a vector
void ComponentRigidBody::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// Gets the transform of the Rigidbody
void ComponentRigidBody::GetTransform(float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// Sets the transform of the Rigidbody
void ComponentRigidBody::SetTransform(const float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// Sets the position of the Rigidbody
void ComponentRigidBody::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}

// Rotates the Rigidbody
void ComponentRigidBody::RotateBody(btQuaternion rotationQuaternion) {

	btTransform transform = body->getWorldTransform();

	transform.setRotation(rotationQuaternion);
	body->setWorldTransform(transform);
}
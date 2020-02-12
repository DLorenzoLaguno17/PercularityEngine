#include "Application.h"
#include "ModulePhysics.h"
#include "ComponentRigidBody.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Time.h"
#include "ModuleScene.h"

#include "mmgr/mmgr.h"

ComponentRigidBody::ComponentRigidBody(GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::RIGIDBODY, parent, active) {}

ComponentRigidBody::ComponentRigidBody(btRigidBody* body) : body(body)
{
	body->setUserPointer(this);
}

void ComponentRigidBody::Update() {
	if (gameObject != nullptr)
	{
		if (Time::running || active == false)
		{
			mat4x4 bodyTransform, parentTransform, newTransform;
			body->getWorldTransform().getOpenGLMatrix(&bodyTransform);				

			if (gameObject->parent != nullptr)
				parentTransform = gameObject->parent->transform->GetGlobalGLTransform();
			else
				parentTransform = IdentityMatrix;

			newTransform = parentTransform.inverse() * bodyTransform;

			gameObject->transform->SetLocalTransform(newTransform);
			gameObject->transform->Move(-localPosition);
		}
	}
}

void ComponentRigidBody::OnEditor()
{
	if (ImGui::CollapsingHeader("RigidBody")) {

		ImGui::Checkbox("Enabled", &active);
		if (ImGui::Button("Set to zero"))
		{
			localPosition = float3(0.0f, 0.0f, 0.0f);
			OnUpdateTransform();
		}

		if (ImGui::DragFloat3("Local Position", (float*)&localPosition, 0.1))
			OnUpdateTransform();
		
		if (ImGui::Checkbox("Dynamic object", &dynamicObject))
		{
			if (!dynamicObject)
			{
				App->physics->world->removeRigidBody(body);

				btVector3 localInertia(0, 0, 0);
				body->getCollisionShape()->calculateLocalInertia(0.0f, localInertia);
				body->setMassProps(0.0f, localInertia);

				App->physics->world->addRigidBody(body);
			}
			else
			{
				App->physics->world->removeRigidBody(body);

				btVector3 localInertia(0, 0, 0);
				body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
				body->setMassProps(mass, localInertia);

				App->physics->world->addRigidBody(body);
			}
		}
		
		if (dynamicObject)
		{
			if (ImGui::DragFloat("Mass of the building: %.2f", &mass))
			{
				App->physics->world->removeRigidBody(body);

				btVector3 localInertia(0, 0, 0);
				body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
				body->setMassProps(mass, localInertia);

				App->physics->world->addRigidBody(body);
			}
		}
	}
}

void ComponentRigidBody::CreateBody(btRigidBody* body)
{
	this->body = body;
	body->setUserPointer(this);
}

void ComponentRigidBody::CleanUp() 
{
	App->physics->world->removeCollisionObject(body);
	RELEASE(body);
}

// Load & Save 
void ComponentRigidBody::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file)
{
	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Active"];
	mass = scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Mass"];

	App->physics->AddRigidBody(gameObject->obb, gameObject, mass);

	float3 pos;
	pos.x = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Offset"].at(0);
	pos.y = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Offset"].at(1);
	pos.z = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Offset"].at(2);
	localPosition = pos;
}

void ComponentRigidBody::OnSave(const char* gameObjectNum, nlohmann::json &scene_file)
{
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Active"] = active;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Rigidbody"]["Mass"] = mass;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Offset"] = { localPosition.x, localPosition.y, localPosition.y };
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

	btTransform t = body->getWorldTransform();
	t.setRotation(rotationQuaternion);
	body->setWorldTransform(t);
}

void ComponentRigidBody::OnUpdateTransform()
{
	if (!Time::running || followObject)
	{
		float4x4 objectTransform = gameObject->transform->GetGlobalTransform();
		objectTransform = objectTransform * objectTransform.Translate(localPosition);

		mat4x4 glTransform;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				glTransform.M[i * 4 + j] = objectTransform[j][i];

		btTransform newTransform;
		newTransform.setFromOpenGLMatrix(&glTransform);
		body->setWorldTransform(newTransform);
	}
}
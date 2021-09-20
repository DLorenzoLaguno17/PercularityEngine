#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#include "ComponentTransform.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "ImGui/ImGuizmo.h"

ComponentTransform::ComponentTransform(GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::TRANSFORM, parent, active) 
{
	translationM = GetTranslation();
	rotationM = GetEulerRotation();
	scaleM = GetScale();
}

void ComponentTransform::Update()
{
	UpdateTransform();

	// Check if the transform has changed, and record the action if it has
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && gameObject == App->scene->selected)
	{
		if (!lastTranslation.Equals(translation))
		{
			TranslateGameObject* translationAction = new TranslateGameObject(lastTranslation, translation, this);
			App->undo->StoreNewAction(translationAction);

			lastTranslation = translation;
			firstTime = false;
		}

		if (!lastRotation.Equals(eulerRotation))
		{
			RotateGameObject* rotationAction = new RotateGameObject(lastRotation, eulerRotation, this);
			App->undo->StoreNewAction(rotationAction);

			lastRotation = eulerRotation;
			firstTime = false;
		}

		else if (!lastScale.Equals(scale))
		{
			ScaleGameObject* scaleAction = new ScaleGameObject(lastScale, scale, this);
			App->undo->StoreNewAction(scaleAction);

			lastScale = scale;
			firstTime = false;
		}
	}
}

void ComponentTransform::OnEditor() 
{
	if (ImGui::CollapsingHeader("Transform")) 
	{
		float4x4 model = gameObject->transform->GetLocalTransform();
		model.Transpose();

		translationM = GetTranslation();
		rotationM = GetEulerRotation();
		scaleM = GetScale();

		if (ImGui::Button("Reset transform"))
			SetToZero();

		if (ImGui::DragFloat3("Translation", (float*)&translationM, 0.3))
			SetPosition(translationM);

		if (ImGui::DragFloat3("Rotation", (float*)&rotationM, 0.3))
			SetEulerRotation(rotationM);

		if (ImGui::DragFloat3("Scale", (float*)&scaleM, 0.05))
			SetScale(scaleM);

		ImGui::NewLine();
	}
}

void ComponentTransform::UpdateTransform()
{
	if (mustUpdate)
	{
		localTransform = float4x4::FromTRS(translation, rotation, scale);

		if (strcmp("World", gameObject->name.c_str()) != 0 && gameObject->parent != nullptr)
			globalTransform = gameObject->parent->transform->GetGlobalTransform() * localTransform;
		else
			globalTransform = localTransform;

		gameObject->UpdateAABB();

		for (int i = 0; i < gameObject->children.size(); ++i)
			gameObject->children[i]->transform->UpdateTransform();

		for (int i = 0; i < gameObject->components.size(); ++i)
			gameObject->components[i]->OnUpdateTransform();

		if (gameObject->isStatic) 
		{
			App->scene->sceneTree->Erase(gameObject);
			App->scene->sceneTree->Insert(gameObject);
		}

		mustUpdate = false;
	
		for (int i = 0; i < gameObject->children.size(); ++i)
			gameObject->children[i]->transform->mustUpdate = true;
	}
}

const mat4x4 ComponentTransform::GetGlobalGLTransform() const
{
	mat4x4 ret;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			ret.M[i * 4 + j] = globalTransform[j][i];

	return ret;
}

void ComponentTransform::Move(float3 positionIncrease)
{
	translation += positionIncrease;
	mustUpdate = true;
}

void ComponentTransform::Scale(float3 scale_)
{
	scale += scale_;
	mustUpdate = true;
}

void ComponentTransform::SetLocalTransform(float4x4 transform)
{
	transform.Decompose(translation, rotation, scale);
	eulerRotation = rotation.ToEulerXYZ().Abs();
	mustUpdate = true;
}

void ComponentTransform::SetGlobalTransform(float4x4 transform)
{
	globalTransform = transform;
}

void ComponentTransform::SetGlobalTransform(mat4x4 transform)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			globalTransform[j][i]=transform.M[i * 4 + j];
}

void ComponentTransform::SetEulerRotation(float3 eulerAngle)
{
	if (firstTime)
	{
		lastRotation = eulerRotation;
		firstTime = false;
	}

	float3 angleIncrease = (eulerAngle - eulerRotation)*DEGTORAD;
	Quat newRotation = Quat::FromEulerXYZ(angleIncrease.x, angleIncrease.y, angleIncrease.z);
	rotation = rotation * newRotation;
	eulerRotation = eulerAngle;
	mustUpdate = true;
}

void ComponentTransform::SetLocalTransform(mat4x4 transform)
{
	float4x4 newTransform;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			newTransform[j][i] = transform.M[i * 4 + j];

	newTransform.Decompose(translation, rotation, scale);
	eulerRotation = rotation.ToEulerXYZ().Abs();
	mustUpdate = true;
}

void ComponentTransform::SetPosition(float3 newPosition)
{
	if (firstTime)
	{
		lastTranslation = translation;
		firstTime = false;
	}

	translation = newPosition;
	mustUpdate = true;
}

void ComponentTransform::SetScale(float3 newScale)
{
	if (firstTime)
	{
		lastScale = scale;
		firstTime = false;
	}

	scale = newScale;
	mustUpdate = true;
}

void ComponentTransform::SetRotation(Quat newRotation) {
	
	rotation = newRotation;
	UpdateEulerRotation();
	mustUpdate = true;
}

void ComponentTransform::SetToZero()
{
	scale = { 1,1,1 };
	translation = { 0,0,0 };
	rotation = Quat::identity;
	UpdateEulerRotation();

	mustUpdate = true;
}

void ComponentTransform::UpdateEulerRotation()
{
	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;
}

// Load & Save 
void ComponentTransform::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file) {
	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Active"];	

	Quat rot;
	rot.x = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Rotation"].at(0);
	rot.y = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Rotation"].at(1);
	rot.z = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Rotation"].at(2);
	rot.w = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Rotation"].at(3);
	SetRotation(rot);

	float3 tra;
	tra.x = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Translation"].at(0);
	tra.y = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Translation"].at(1);
	tra.z = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Translation"].at(2);
	SetPosition(tra);

	float3 sca;
	sca.x = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Scale"].at(0);
	sca.y = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Scale"].at(1);
	sca.z = scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Scale"].at(2);
	SetScale(sca);
}

void ComponentTransform::OnSave(const char* gameObjectNum, nlohmann::json &scene_file) {
	scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Active"] = active;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Rotation"] = {rotation.x, rotation.y, rotation.z, rotation.w };
	scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Translation"] = { translation.x, translation.y, translation.z };
	scene_file["Game Objects"][gameObjectNum]["Components"]["Transform"]["Scale"] = { scale.x, scale.y, scale.y };
}

// --------------------------------------------------
void TranslateGameObject::Undo()
{
	transform->SetPosition(lastPosition);
	transform->lastTranslation = transform->GetTranslation();
}

void TranslateGameObject::Redo()
{
	transform->SetPosition(newPosition);
	transform->lastTranslation = transform->GetTranslation();
}

void RotateGameObject::Undo()
{
	transform->SetEulerRotation(lastAngles);
	transform->lastRotation = transform->GetEulerRotation();
}

void RotateGameObject::Redo()
{
	transform->SetEulerRotation(newAngles);
	transform->lastRotation = transform->GetEulerRotation();
}

void ScaleGameObject::Undo()
{
	transform->SetScale(lastScale);
	transform->lastScale = transform->GetScale();
}

void ScaleGameObject::Redo()
{
	transform->SetScale(newScale);
	transform->lastScale = transform->GetScale();
}
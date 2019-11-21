#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ModuleResourceLoader.h"
#include "ModuleInput.h"

//test
#include "ComponentCamera.h"

#include <fstream>
#include <iomanip>

#define PAR_SHAPES_IMPLEMENTATION

#include "Par Shapes/par_shapes.h"
#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Init() {
	root = new GameObject("World");
	sceneAddress = "Assets/Scenes/";
	sceneExtension = ".json";

	frustumTest = new GameObject();
	frustumTest->CreateComponent(COMPONENT_TYPE::CAMERA);

	objectTree = new Tree(TREE_TYPE::OCTREE, AABB({ -50,-50,-50 }, { 50,50,50 }),1);

	return true;
}

bool ModuleScene::Start()
{
	// Loading FBX
	App->res_loader->LoadModel("Assets/FBX/BakerHouse.fbx");



	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("ScenePreUpdate", Profiler::Color::Orange);	

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	BROFILER_CATEGORY("SceneUpdate", Profiler::Color::LightSeaGreen);

	DrawSimplePlane();
	DrawAxis();

	// Update all GameObjects
	UpdateGameObjects(root);

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN
		&& ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) 
		|| (App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)))
		SaveScene("Test");

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		ImGui::OpenPopup("Loading new scene");
	}

	if (ImGui::BeginPopupModal("Loading new scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure you want to load a new scene?");
		ImGui::Text("The current scene will be discarded.");
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("Yes", ImVec2(140, 0))) { ImGui::CloseCurrentPopup(); mustLoad = true; }
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(140, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	if (mustLoad) LoadScene("Test");
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		for (int i = 0; i < root->children.size(); ++i)
			objectTree->rootNode->Insert(root->children[i]);

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ScenePostUpdate", Profiler::Color::Yellow);

	objectTree->Draw();
		frustumTest->GetComponent<ComponentCamera>()->DrawFrustum();


	return UPDATE_CONTINUE;
}

void ModuleScene::UpdateGameObjects(GameObject* root) {
	root->Update();

	for (uint i = 0; i < root->children.size(); ++i)
		UpdateGameObjects(root->children[i]);
}

// Deletes all the allocated data
bool ModuleScene::CleanUp()
{
	LOG("Releasing all the GameObjects");
	RecursiveCleanUp(root);
	objectTree->Clear();

	return true;
}

void ModuleScene::RecursiveCleanUp(GameObject* root) {
	for (int i = 0; i < root->children.size(); ++i)
		RecursiveCleanUp(root->children[i]);

	RELEASE(root);
}

void ModuleScene::LoadScene(const std::string scene_name) {
	
	// First we delete the current scene
	CleanUp();
	root = new GameObject("World", nullptr, true);

	json scene_file;
	std::string full_path = sceneAddress + scene_name + sceneExtension;

	// If the adress of the settings file is null, create  an exception
	assert(full_path.c_str() != nullptr);

	// Create a stream and open the file
	std::ifstream stream;
	stream.open(full_path);
	scene_file = json::parse(stream);

	// Close the file
	stream.close();

	numGameObjectsInScene = scene_file["Game Objects"]["Count"];
	RecursiveLoad(root, scene_file);
	selected = root;
	loaded_go = 0;
	mustLoad = false;
}

void ModuleScene::RecursiveLoad(GameObject* root, const nlohmann::json &scene_file) {
	loaded_go++;
	char name[50];
	sprintf_s(name, 50, "GameObject %d", loaded_go);
	root->OnLoad(name, scene_file);

	for (int i = 1; i <= numGameObjectsInScene; ++i) {

		char n[50];
		sprintf_s(n, 50, "GameObject %d", i);
		uint aux = scene_file["Game Objects"][n]["Parent UUID"];

		if (aux == root->GetUUID() && i != 1) { //The first one is the World itself so we skip it
			GameObject* newGo;
			newGo = new GameObject("Temp", root, true);
		}
	}	

	for (int i = 0; i < root->children.size(); ++i)
		RecursiveLoad(root->children[i], scene_file);
}

void ModuleScene::SaveScene(std::string scene_name) {

	// Create auxiliar file
	json scene_file;
	std::string full_path = sceneAddress + scene_name + sceneExtension;

	RecursiveSave(root, scene_file);
	scene_file["Game Objects"]["Count"] = saved_go;
	saved_go = 0;

	// Create the stream and open the file
	std::ofstream stream;
	stream.open(full_path);
	stream << std::setw(4) << scene_file << std::endl;
	stream.close();
}

void ModuleScene::RecursiveSave(GameObject* root, nlohmann::json &scene_file) {
	saved_go++;
	char name[50];
	sprintf_s(name, 50, "GameObject %d", saved_go);
	root->OnSave(name, scene_file);

	for (int i = 0; i < root->children.size(); ++i)
		RecursiveSave(root->children[i], scene_file);
}

void ModuleScene::DrawSimplePlane() const
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(255, 255, 255);

	// Try to draw a plane
	for (float i = -100.0f; i < 100; ++i)
	{
		glVertex3f(i, 0.0f, -100.0f);
		glVertex3f(i, 0.0f, 100.0f);
	}

	for (float i = -100.0f; i < 100; ++i)
	{
		glVertex3f(-100, 0.0f, i);
		glVertex3f(100.f, 0.0f, i);
	}

	glEnd();
}

void ModuleScene::DrawAxis() const {

	glLineWidth(7.0f);

	glBegin(GL_LINES);

	//X axis
	glColor3f(1, 0, 0); //Red color

	glVertex3f(-0.1f, 0.1f, -0.1f);
	glVertex3f(5.1f, 0.1f, -0.1f);

	///Draw an X
	glVertex3f(5.0f, 2.0f, -0.2f);
	glVertex3f(4.5f, 1.5f, -0.2f);
	glVertex3f(4.5f, 2.0f, -0.2f);
	glVertex3f(5.0f, 1.5f, -0.2f);

	//Y axis
	glColor3f(0, 1, 0); //Green color

	glVertex3f(-0.1f, 0.1f, -0.1f);
	glVertex3f(-0.1f, 5.1f, -0.1f);

	///Draw an Y
	glVertex3f(0.0f, 5.5f, 0.0f);
	glVertex3f(0.0f, 6.0f, 0.0f); 
	glVertex3f(0.0f, 6.0f, 0.0f);
	glVertex3f(-0.25f, 6.5f, 0.0f);
	glVertex3f(0.0f, 6.0f, 0.0f);
	glVertex3f(0.15f, 6.5f, 0.0f);

	//Z axis
	glColor3f(0, 0, 1); //Blue color

	glVertex3f(-0.1f, 0.1f, -0.1f);
	glVertex3f(-0.1f, 0.1f, 5.1f);

	///Draw a Z
	glVertex3f(0.0f, 2.0f, 4.5f);
	glVertex3f(0.0f, 2.0f, 5.0f);
	glVertex3f(0.0f, 2.0f, 5.0f);
	glVertex3f(0.0f, 1.5f, 4.5f);
	glVertex3f(0.0f, 1.5f, 4.5f);
	glVertex3f(0.0f, 1.5f, 5.0f);

	glEnd();

	glColor3f(1, 1, 1);//Set color back to white
}

GameObject* ModuleScene::CreateSphere(int slices, int stacks, float diameter)
{
	GameObject* item = new GameObject();

	if (sphereCount > 0) {
		char name[50];
		sprintf_s(name, 50, "Sphere (%d)", sphereCount);
		item->name = name;
	}
	else item->name = "Sphere";
	sphereCount++;

	ComponentMesh* mesh = (ComponentMesh*)item->CreateComponent(COMPONENT_TYPE::MESH);
	ComponentMaterial* material = (ComponentMaterial*)item->CreateComponent(COMPONENT_TYPE::MATERIAL);

	//Create the mesh with Par_Shapes
	par_shapes_mesh_s* newMesh;
	newMesh = par_shapes_create_parametric_sphere(slices,stacks);
	par_shapes_scale(newMesh, diameter, diameter, diameter);

	//Convert the par_Mesh into a regular mesh
	mesh->LoadParShape(newMesh);

	//Free the Par_mesh
	par_shapes_free_mesh(newMesh);

	//Set default texture
	material->texture = App->res_loader->default_tex;
	App->scene->selected = item;
	numGameObjectsInScene++;

	return item;
}

GameObject* ModuleScene::CreateCube(float sizeX, float sizeY, float sizeZ)
{
	GameObject* item = new GameObject();

	if (cubeCount > 0) {
		char name[50];
		sprintf_s(name, 50, "Cube (%d)", cubeCount);
		item->name = name;
	}
	else item->name = "Cube";
	cubeCount++;

	ComponentMesh* mesh = (ComponentMesh*)item->CreateComponent(COMPONENT_TYPE::MESH);
	ComponentMaterial* material = (ComponentMaterial*)item->CreateComponent(COMPONENT_TYPE::MATERIAL);

	//Create the mesh with Par_Shapes
	//create 6 faces of the cube (we don't use par_shapes_create_cube
	//because it doesn't have normals or uvs
	par_shapes_mesh_s* mesh_front =par_shapes_create_plane(1,1);
	par_shapes_mesh_s* back = par_shapes_create_plane(1, 1);
	par_shapes_mesh_s* right = par_shapes_create_plane(1, 1);
	par_shapes_mesh_s* left = par_shapes_create_plane(1, 1);
	par_shapes_mesh_s* top = par_shapes_create_plane(1, 1);
	par_shapes_mesh_s* bottom = par_shapes_create_plane(1, 1);

	par_shapes_translate(back, -0.5f, -0.5f, 0.5f);

	par_shapes_rotate(mesh_front, PAR_PI, (float*)&float3::unitY);
	par_shapes_translate(mesh_front, 0.5f, -0.5f, -0.5f);

	par_shapes_rotate(left, PAR_PI/2, (float*)&float3::unitY);
	par_shapes_translate(left, 0.5f, -0.5f, 0.5f);

	par_shapes_rotate(right, -PAR_PI / 2, (float*)&float3::unitY);
	par_shapes_translate(right, -0.5f, -0.5f, -0.5f);

	par_shapes_rotate(bottom, PAR_PI / 2, (float*)&float3::unitX);
	par_shapes_translate(bottom, -0.5f, -0.5f, -0.5f);
	
	par_shapes_rotate(top, -PAR_PI / 2, (float*)&float3::unitX);
	par_shapes_translate(top, -0.5f, 0.5f, 0.5f);

	par_shapes_merge_and_free(mesh_front, back);
	par_shapes_merge_and_free(mesh_front, left);
	par_shapes_merge_and_free(mesh_front,right);
	par_shapes_merge_and_free(mesh_front, bottom);
	par_shapes_merge_and_free(mesh_front, top);
	
	par_shapes_scale(mesh_front, sizeX, sizeY, sizeZ);

	//Convert the par_Mesh into a regular mesh
	mesh->LoadParShape(mesh_front);

	//Free the Par_mesh
	par_shapes_free_mesh(mesh_front);

	//Set default texture
	material->texture = App->res_loader->default_tex;
	App->scene->selected = item;
	numGameObjectsInScene++;

	return item;
}

GameObject* ModuleScene::CreatePlane(float length, float depth)
{
	GameObject* item = new GameObject();

	if (planeCount > 0) {
		char name[50];
		sprintf_s(name, 50, "Plane (%d)", planeCount);
		item->name = name;
	}
	else item->name = "Plane";
	planeCount++;

	ComponentMesh* mesh = (ComponentMesh*)item->CreateComponent(COMPONENT_TYPE::MESH);
	ComponentMaterial* material = (ComponentMaterial*)item->CreateComponent(COMPONENT_TYPE::MATERIAL);

	//Convert the par_Mesh into a regular mesh
	par_shapes_mesh_s* plane = par_shapes_create_plane(2,2);

	par_shapes_rotate(plane, -PAR_PI / 2, (float*)&float3::unitX);

	par_shapes_scale(plane,length,0.0f, depth);

	//Convert the par_Mesh into a regular mesh
	mesh->LoadParShape(plane);

	//Free the Par_mesh
	par_shapes_free_mesh(plane);

	//Set default texture
	material->texture = App->res_loader->default_tex;
	App->scene->selected = item;
	numGameObjectsInScene++;

	return item;
}

GameObject* ModuleScene::CreateDonut(int slices, int stacks, float radius)
{
	GameObject* item = new GameObject();

	if (donutCount > 0) {
		char name[50];
		sprintf_s(name, 50, "Donut (%d)", donutCount);
		item->name = name;
	}
	else item->name = "Donut";
	donutCount++;

	ComponentMesh* mesh = (ComponentMesh*)item->CreateComponent(COMPONENT_TYPE::MESH);
	ComponentMaterial* material = (ComponentMaterial*)item->CreateComponent(COMPONENT_TYPE::MATERIAL);

	//Create the mesh with Par_Shapes
	par_shapes_mesh_s* newMesh = par_shapes_create_torus(slices, stacks, 0.5f);

	par_shapes_rotate(newMesh, -PAR_PI / 2, (float*)&float3::unitX);

	par_shapes_scale(newMesh, radius, radius, radius);

	//Convert the par_Mesh into a regular mesh
	mesh->LoadParShape(newMesh);

	//Free the Par_mesh
	par_shapes_free_mesh(newMesh);

	//Set default texture
	material->texture = App->res_loader->default_tex;
	App->scene->selected = item;
	numGameObjectsInScene++;

	return item;
}
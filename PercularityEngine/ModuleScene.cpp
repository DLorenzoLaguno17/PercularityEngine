#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ModuleResourceLoader.h"

#define PAR_SHAPES_IMPLEMENTATION

#include "Par Shapes/par_shapes.h"
#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Init() {
	App->scene->root = new GameObject("World");
	return true;
}

bool ModuleScene::Start()
{
	// Loading FBX
	App->res_loader->LoadFBX("Assets/FBX/BakerHouse.fbx");
	loadedAll = true;

	if (!game_objects.empty())
		selected = game_objects.back();

	return true;
}

void ModuleScene::Load(const nlohmann::json &config) {
	for (int i = 0; i < game_objects.size(); ++i)
		game_objects[i]->OnLoad(config);
}

void ModuleScene::Save(nlohmann::json &config) {
	for (int i = 0; i < game_objects.size(); ++i)
		game_objects[i]->OnSave(config);
}

update_status ModuleScene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("ScenePreUpdate", Profiler::Color::Orange);

	

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	BROFILER_CATEGORY("SceneUpdate", Profiler::Color::LightSeaGreen);

	//Draw a plane
	DrawSimplePlane();

	//Test
	DrawAxis();

	// Draw all GameObjects
	for (uint i = 0; i < game_objects.size(); ++i) {
		game_objects[i]->Update();
	}
	
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ScenePostUpdate", Profiler::Color::Yellow)

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	// Delete all the GameObjects
	for (uint i = 0; i < game_objects.size(); ++i) {
		game_objects[i]->CleanUp();
		delete game_objects[i];
	}

	game_objects.clear();
	delete root;

	root = nullptr;

	return true;
}

uint ModuleScene::GenerateRandomUUID() {
	return 0;
}

void ModuleScene::DrawSimplePlane()const
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(255, 255, 255);

	//try to draw a plane
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
	GameObject* item=new GameObject();
	item->name = "Sphere";

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

	//Add the object to the list
	App->scene->game_objects.push_back(item);

	return item;
}

GameObject* ModuleScene::CreateCube(float sizeX, float sizeY, float sizeZ)
{
	GameObject* item = new GameObject();
	item->name = "Cube";

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

	//Add the object to the list
	App->scene->game_objects.push_back(item);

	return item;
}

GameObject* ModuleScene::CreatePlane(float length, float depth)
{
	GameObject* item = new GameObject();
	item->name = "Plane";

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

	//Add the object to the list
	App->scene->game_objects.push_back(item);

	return item;
}

GameObject* ModuleScene::CreateDonut(int slices, int stacks, float radius)
{
	GameObject* item = new GameObject();
	item->name = "Donut";

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

	//Add the object to the list
	App->scene->game_objects.push_back(item);

	return item;
}
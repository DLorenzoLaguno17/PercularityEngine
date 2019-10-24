#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"
#include "Primitive.h"
#include "GameObject.h"

#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled):Module(app, start_enabled)
{}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	selected = &game_objects.front();
	selected_id = game_objects.size() - 1;

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("ScenePreUpdate", Profiler::Color::Orange)

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	BROFILER_CATEGORY("SceneUpdate", Profiler::Color::LightSeaGreen);

	//Draw primitives
	for (int i = 0; i < scenePrimitives.size(); ++i)
		scenePrimitives[i]->Render();

	//Draw a plane
	DrawSimplePlane();

	//Test
	DrawAxis();

	// Draw all models
	for (uint i = 0; i < game_objects.size(); ++i) {
		game_objects[i].Render();
		if (normalsShown) game_objects[i].ShowNormals();
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
	// Delete all the primitives
	for (int i = 0; i < scenePrimitives.size(); ++i)
		delete scenePrimitives[i];

	scenePrimitives.clear();

	// Delete all the GameObjects
	for (uint i = 0; i < game_objects.size(); ++i) {
		game_objects[i].CleanUp();
	}

	game_objects.clear();

	return true;
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
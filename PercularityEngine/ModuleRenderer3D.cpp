#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleGui.h"
#include <stdio.h>
#include <iostream>

#include "OpenGL.h"
#include "Brofiler/Lib/Brofiler.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(10.0f,10.0f, 10.0f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}

	// Projection matrix for
	OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());

	CreateCube();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("RendererPreUpdate", Profiler::Color::Orange)

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("RendererPostUpdate", Profiler::Color::Yellow)

	App->gui->DrawImGui(dt);	/*Shouldn't really be here, 
								should find a better way to 
								order module drawing  Joan M*/

	SDL_GL_SwapWindow(App->window->window);

	return status;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::DrawSimplePlane()const
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);

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

void ModuleRenderer3D::DrawDirectCube()const {

	glBegin(GL_TRIANGLES);

	glRotatef(0.1f, 1.0f, 1.0f, 0.0f);

	//Bottom face
	glVertex3f(5.0f, 5.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 5.0f);

	glVertex3f(0.0f, 5.0f, 5.0f);
	glVertex3f(5.0f, 5.0f, 5.0f);
	glVertex3f(5.0f, 5.0f, 0.0f);

	//Top face
	glVertex3f(5.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 5.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glVertex3f(0.0f, 0.0f, 5.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 5.0f);

	//right face
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 5.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 5.0f);
	glVertex3f(0.0f, 5.0f, 5.0f);

	//left face
	glVertex3f(5.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 5.0f, 0.0f);
	glVertex3f(5.0f, 5.0f, 5.0f);

	glVertex3f(5.0f, 0.0f, 0.0f);
	glVertex3f(5.0f, 5.0f, 5.0f);
	glVertex3f(5.0f, 0.0f, 5.0f);

	//front face
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);

	glVertex3f(5.0f, 5.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);

	//back face
	glVertex3f(0.0f, 0.0f, 5.0f);
	glVertex3f(5.0f, 0.0f, 5.0f);
	glVertex3f(0.0f, 5.0f, 5.0f);

	glVertex3f(5.0f, 5.0f, 5.0f);
	glVertex3f(0.0f, 5.0f, 5.0f);
	glVertex3f(5.0f, 0.0f, 5.0f);

	glEnd();

}

void ModuleRenderer3D::DrawAxis() const{
	
	glLineWidth(5.0f);
	
	glBegin(GL_LINES);
	
	//X axis
	glColor3f(1, 0, 0); //Red color

	glVertex3f(-0.1f, -0.1f, -0.1f);
	glVertex3f(5.1f, -0.1f, -0.1f);

	//Y axis
	glColor3f(0, 1, 0); //Green color

	glVertex3f(-0.1f, -0.1f, -0.1f);
	glVertex3f(-0.1f, 5.1f, -0.1f);

	//Z axis
	glColor3f(0, 0, 1); //Blue color

	glVertex3f(-0.1f, -0.1f, -0.1f);
	glVertex3f(-0.1f, -0.1f, 5.1f);

	glEnd();

	glColor3f(1, 1, 1);//Set color back to white
}

void ModuleRenderer3D::CreateCube()
{
	
	//create a list with the vertices
	std::vector<vec3> vertexList;

	vertexList.push_back(vec3(0.0f, 0.0f, 0.0f));
	vertexList.push_back(vec3(0.0f, 0.0f, 5.0f));
	vertexList.push_back(vec3(0.0f, 5.0f, 5.0f));

}
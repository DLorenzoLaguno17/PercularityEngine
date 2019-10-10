#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleGui.h"
#include <stdio.h>
#include "ModuleScene.h"
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

	return ret;
}

bool ModuleRenderer3D::Start()
{
	//testing
	CreateRenderingData();

	//test
	SetUpScene();
	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("RendererPreUpdate", Profiler::Color::Orange)

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

	//Test
	Render();

	
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

GLfloat vertices2[] = {
5.0f, 5.0f, 0.0f,
0.0f, 5.0f, 0.0f,
0.0f, 5.0f, 5.0f,

0.0f, 5.0f, 5.0f,
5.0f, 5.0f, 5.0f,
5.0f, 5.0f, 0.0f,


5.0f, 0.0f, 0.0f,
0.0f, 0.0f, 5.0f,
0.0f, 0.0f, 0.0f,

0.0f, 0.0f, 5.0f,
5.0f, 0.0f, 0.0f,
5.0f, 0.0f, 5.0f,


0.0f, 0.0f, 0.0f,
0.0f, 5.0f, 5.0f,
0.0f, 5.0f, 0.0f,

0.0f, 0.0f, 0.0f,
0.0f, 0.0f, 5.0f,
0.0f, 5.0f, 5.0f,


5.0f, 0.0f, 0.0f,
5.0f, 5.0f, 0.0f,
5.0f, 5.0f, 5.0f,

5.0f, 0.0f, 0.0f,
5.0f, 5.0f, 5.0f,
5.0f, 0.0f, 5.0f,


0.0f, 0.0f, 0.0f,
0.0f, 5.0f, 0.0f,
5.0f, 0.0f, 0.0f,

5.0f, 5.0f, 0.0f,
5.0f, 0.0f, 0.0f,
0.0f, 5.0f, 0.0f,

0.0f, 0.0f, 5.0f,
5.0f, 0.0f, 5.0f,
0.0f, 5.0f, 5.0f,

5.0f, 5.0f, 5.0f,
0.0f, 5.0f, 5.0f,
5.0f, 0.0f, 5.0f, };

GLfloat vertices[] =
{
	15.f,5.f,5.f,
	5.f,5.f,5.f,
	15.f,15.f,5.f,
	5.f,15.f,5.f,

	15.f,5.f,15.f,
	5.f,5.f,15.f,
	15.f,15.f,15.f,
	5.f,15.f,15.f
};

GLubyte indices[] =
{
	0,1,2, 1,3,2,
	1,5,3, 3,5,7,
	3,7,6, 3,6,2,
	4,0,2, 4,2,6,
	0,5,1, 0,4,5,
	7,5,4, 7,4,6
};

void ModuleRenderer3D::CreateRenderingData()
{
	//Create buffer for the vertices
	glGenBuffers(1, (GLuint*) &(verticesBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Create buffer for the vertices
	glGenBuffers(1, (GLuint*) &(verticesBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Create buffer for the indices
	glGenBuffers(1, (GLuint*) &(indicesBuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleRenderer3D::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	//Draw a cube w/o indices
	glEnableClientState(GL_VERTEX_ARRAY);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices2);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableClientState(GL_VERTEX_ARRAY);
	//Draw a cube w/o indices
	
	//Draw a cube with indices
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);				//vertices buffer

	glVertexPointer(3, GL_FLOAT, 0, NULL);			//specfy pointer to vertices coords

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);		//indices buffer

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE,nullptr);//draw elements

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	//Draw a cube with indices

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void ModuleRenderer3D::SetUpScene()
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Generate texture
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
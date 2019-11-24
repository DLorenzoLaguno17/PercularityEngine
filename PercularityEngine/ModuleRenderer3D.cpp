#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "ModuleGui.h"
#include <stdio.h>
#include <iostream>
#include "OpenGL.h"
#include "Brofiler/Lib/Brofiler.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "Intersection.h"
#include "GameObject.h"

//test
#include"ModuleScene.h"

#include "mmgr/mmgr.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "GLEW/lib86/glew32.lib")
//#pragma comment( lib, "GLEW/lib86/glew32s.lib" )

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled) {}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D() {}

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
		// Initialize glew
		GLenum error = glewInit();
		LOG("Using Glew %s", glewGetString(GLEW_VERSION));

		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing glew! %s\n", glewGetErrorString(error));
			ret = false;
		}

		//To detect our current hardware and driver capabilities
		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

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

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

		lights[0].Active(true);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}

	// Projection matrix for

	return ret;
}

bool ModuleRenderer3D::Start()
{
	OnResize(App->window->GetWindowWidth(), App->window->GetWindowHeight());
	//Prepare scene
	SetUpScene();

	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("RendererPreUpdate", Profiler::Color::Orange);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	if (camera->update_projection) {
		UpdateProjectionMatrix();
		camera->update_projection = false;
	}

	glLoadMatrixf(camera->GetOpenGLViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(camera->frustum.pos.x, camera->frustum.pos.y, camera->frustum.pos.z);
	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();


	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("RendererPostUpdate", Profiler::Color::Yellow);

	DrawMeshes();

	// Reset the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the Gui
	App->gui->DrawImGui(dt);

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

	camera->SetAspectRatio( float(width)/ float(height));
	UpdateProjectionMatrix();
	LOG("Window resized: width: %d ,  height: %d ", width, height);
	LOG("Window resized: width: %d ,  height: %d ", App->window->GetWindowWidth(), App->window->GetWindowHeight());
	LOG("Camera aspect ratio: %f", camera->GetAspectRatio());


	SetUpScene();
}

void ModuleRenderer3D::SetUpScene()
{
	DeleteBuffers();

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Generate texture
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->GetWindowWidth(), App->window->GetWindowHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->GetWindowWidth(), App->window->GetWindowHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint ModuleRenderer3D::CreateBuffer(uint bufferType, uint size, void* data)
{
	uint bufferID;

	glGenBuffers(1, (GLuint*)&(bufferID));
	glBindBuffer(bufferType, bufferID);
	glBufferData(bufferType, size, data, GL_STATIC_DRAW);
	glBindBuffer(bufferType, 0);

	return bufferID;
}

void ModuleRenderer3D::UpdateProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf((GLfloat*)camera->GetOpenGLProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::DeleteBuffers()
{
	if (frameBuffer != 0) {
		glDeleteFramebuffers(1, &frameBuffer);
		frameBuffer = 0;
	}

	if (renderBuffer != 0) {
		glDeleteRenderbuffers(1, &renderBuffer);
		renderBuffer = 0;
	}
	if (texColorBuffer != 0)
	{
		glDeleteTextures(1, &texColorBuffer);
		texColorBuffer = 0;
	}
}

void ModuleRenderer3D::DrawMeshes()
{
	static Frustum* frustum = &App->scene->frustumTest->GetComponent<ComponentCamera>()->frustum;

	if (!frustumCullingActive)
	{
		for (int i = 0; i < meshes.size(); ++i)
			meshes[i]->Render();
		return;
	}
	else
	{
		if (!acceleratedCullingActive)
		{
			for (int i = 0; i < meshes.size(); ++i)
			{
				if (Intersect(*frustum, meshes[i]->gameObject->aabb))
					meshes[i]->Render();
			}
		}
		else
		{
			// Get objects from the sceneTree
			std::vector<const GameObject*> objects;
			App->scene->sceneTree->CollectChilldren(App->scene->frustumTest->GetComponent<ComponentCamera>()->frustum, objects);
			int a = objects.size();

			// Get non static objects
			for (int i = 0; i < App->scene->nonStaticObjects.size(); ++i)
				objects.push_back(App->scene->nonStaticObjects[i]);

			for (int i = 0; i < objects.size(); ++i)
			{
				if (Intersect(*frustum, objects[i]->aabb))
				{
					ComponentMesh* mesh = (ComponentMesh*)objects[i]->GetComponent(COMPONENT_TYPE::MESH);
					if (mesh) mesh->Render();
				}
			}
		}
	}
}

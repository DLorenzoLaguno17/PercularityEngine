#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"

#include "Brofiler/Lib/Brofiler.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled):Module(app, start_enabled)
{}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Generate texture
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	//Draw a plane
	App->renderer3D->DrawSimplePlane();

	//Test 
	App->renderer3D->DrawDirectCube();

	//Test
	App->renderer3D->DrawAxis();
	
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	// Back to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	return true;
}

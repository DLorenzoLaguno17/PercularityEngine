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
	BROFILER_CATEGORY("ScenePreUpdate", Profiler::Color::Orange)

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	BROFILER_CATEGORY("SceneUpdate", Profiler::Color::LightSeaGreen)

	//Draw a plane
	DrawSimplePlane();

	//Test
	DrawAxis();
	
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ScenePostUpdate", Profiler::Color::Yellow)

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

void ModuleScene::DrawSimplePlane()const
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

void ModuleScene::DrawAxis() const {

	glLineWidth(7.0f);

	glBegin(GL_LINES);

	//X axis
	glColor3f(1, 0, 0); //Red color

	glVertex3f(-0.1f, 0.1f, -0.1f);
	glVertex3f(5.1f, 0.1f, -0.1f);

	//Y axis
	glColor3f(0, 1, 0); //Green color

	glVertex3f(-0.1f, 0.1f, -0.1f);
	glVertex3f(-0.1f, 5.1f, -0.1f);

	//Z axis
	glColor3f(0, 0, 1); //Blue color

	glVertex3f(-0.1f, 0.1f, -0.1f);
	glVertex3f(-0.1f, 0.1f, 5.1f);

	glEnd();

	glColor3f(1, 1, 1);//Set color back to white
}
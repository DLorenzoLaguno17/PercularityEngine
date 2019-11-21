#ifndef _MODULER_RENDERER_H__
#define _MODULER_RENDERER_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ComponentCamera;
class ComponentMesh;

class ModuleRenderer3D : public Module
{
	friend class ModuleCamera3D;
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void SetUpScene();
	void DeleteBuffers();

	void UpdateProjectionMatrix();
	void DrawMeshes();

	ComponentCamera* GetCamera() const { return camera; }

public:

	uint CreateBuffer(uint bufferType, uint size, void* data);

	uint GetTexColorBuffer() const	{ return texColorBuffer;}
	uint GetFrameBuffer() const		{ return frameBuffer;	}
	uint GetRenderBuffer() const	{ return renderBuffer;	}

	std::vector<ComponentMesh*> meshes;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	update_status status = UPDATE_CONTINUE;

private:

	bool frustumCullingActive = false;
	bool acceleratedCullingActive = false;

	ComponentCamera* camera;

	uint verticesBuffer		= 0;	//vertex buffer object
	uint indicesBuffer		= 0;	//index
	uint frameBuffer		= 0;
	uint renderBuffer		= 0;
	uint texColorBuffer		= 0;
};

#endif
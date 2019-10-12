#include "Application.h"
#include "ModuleResourceLoader.h"
#include "OpenGL.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "DevIL/include/ilut.h"
#include "Brofiler/Lib/Brofiler.h"

#pragma comment (lib, "Assimp/lib86/assimp.lib")
#pragma comment (lib, "DevIL/lib86/DevIL.lib" )
#pragma comment (lib, "DevIL/lib86/ILU.lib" )
#pragma comment (lib, "DevIL/lib86/ILUT.lib" )

ModuleResourceLoader::ModuleResourceLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleResourceLoader::~ModuleResourceLoader()
{}

// Called before render is available
bool ModuleResourceLoader::Awake()
{
	LOG("Preparing resource loader");
	// Initialize DevIL
	ilInit();
	iluInit();
	
	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);
	ilutRenderer(ILUT_OPENGL);

	ilutInit();

	// DevIL version checking
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION || iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) {
		printf("DevIL version is different...exiting!\n");
		return 1;
	}

	// Enable textures
	glEnable(GL_TEXTURE_2D);

	return true;
}

// Called before the first frame
bool ModuleResourceLoader::Start()
{
	// Stream log messages to Assimp debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	// Loading FBX
	//LoadFBX("Assets/FBX/warrior.FBX"); 
	//LoadFBX("Assets/FBX/demon.fbx");
	//LoadFBX("Assets/FBX/BakerHouse.fbx");

	// Load textures
	CreateTexture("Assets/Textures/lenna.png");

	return true;
}

// Called before Update
update_status ModuleResourceLoader::PreUpdate(float dt)
{
	BROFILER_CATEGORY("ResourceLoaderPreUpdate", Profiler::Color::Orange)

	return UPDATE_CONTINUE;
}

// Called every frame
update_status ModuleResourceLoader::Update(float dt)
{
	BROFILER_CATEGORY("ResourceLoaderUpdate", Profiler::Color::LightSeaGreen)	
	
	for (int i = 0; i < FBX_list.size(); ++i)
		RenderFBX(FBX_list[i]);

	return UPDATE_CONTINUE;
}

// Called after Update
update_status ModuleResourceLoader::PostUpdate(float dt)
{
	BROFILER_CATEGORY("ResourceLoaderPostUpdate", Profiler::Color::Yellow)
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleResourceLoader::CleanUp()
{
	LOG("Freeing mesh loader");
	// Detach Assimp log stream
	aiDetachAllLogStreams();

	for (int i = 0; i < FBX_list.size(); ++i)
		FBX_list[i].clear();

	FBX_list.clear();

	return true;
}

void ModuleResourceLoader::LoadFBX(const char* path) {

	FBX_Mesh mesh;
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; ++i) {

			MeshData* m = new MeshData;

			// Copy vertices
			m->num_vertices = scene->mMeshes[i]->mNumVertices;
			m->vertices = new float[m->num_vertices * 3];
			memcpy(m->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * m->num_vertices * 3);
			LOG("New mesh with %d vertices", m->num_vertices);

			// Copy faces
			if (scene->mMeshes[i]->HasFaces())
			{
				m->num_indices = scene->mMeshes[i]->mNumFaces * 3;
				m->indices = new uint[m->num_indices]; // Assume each face is a triangle
				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3) 
						LOG("WARNING, geometry face with != 3 indices!")
					else 
						memcpy(&m->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(uint));
				}
			}

			// Assigning the VRAM
			glGenBuffers(1, (GLuint*) &m->id_vertex);
			glBindBuffer(GL_ARRAY_BUFFER, m->id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m->num_vertices, m->vertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, (GLuint*) &m->id_index);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 3 * m->num_indices, m->indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			mesh.push_back(m);
		}

		FBX_list.push_back(mesh);
		LOG("Loaded new model. Current FBX on scene: %d", App->res_loader->FBX_list.size());

		aiReleaseImport(scene);
	}
	else LOG("Error loading scene %s", path);
}

void ModuleResourceLoader::RenderFBX(FBX_Mesh mesh) {
	
	for (int i = 0; i < mesh.size(); ++i) {

		glEnableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, tex);
		glBindBuffer(GL_ARRAY_BUFFER, mesh[i]->id_vertex);			 
		glVertexPointer(3, GL_FLOAT, 0, NULL);					 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh[i]->id_index);
		glDrawElements(GL_TRIANGLES, mesh[i]->num_indices, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void ModuleResourceLoader::CreateTexture(const char* path)
{
	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);

	//ilLoadImage(path);
	tex = ilutGLBindTexImage();

	ilBindImage(0);
	ilDeleteImage(ImgId);

	/*glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256,
		0, GL_RGBA, GL_UNSIGNED_BYTE, tex);	

	glBindTexture(GL_TEXTURE_2D, 0);*/
}

void ModuleResourceLoader::Load(const json &config)
{
}

void ModuleResourceLoader::Save(json &config)
{
}
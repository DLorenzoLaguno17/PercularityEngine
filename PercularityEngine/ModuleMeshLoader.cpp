#include "Application.h"
#include "ModuleMeshLoader.h"

#include "OpenGL.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "Brofiler/Lib/Brofiler.h"

#pragma comment (lib, "Assimp/lib86/assimp.lib")
#pragma comment (lib, "DevIL/lib86/DevIL.lib" )
#pragma comment (lib, "DevIL/lib86/ILU.lib" )
#pragma comment (lib, "DevIL/lib86/ILUT.lib" )

ModuleMeshLoader::ModuleMeshLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleMeshLoader::~ModuleMeshLoader()
{}

// Called before render is available
bool ModuleMeshLoader::Awake()
{
	LOG("Preparing mesh loader");

	return true;
}

// Called before the first frame
bool ModuleMeshLoader::Start()
{
	// Stream log messages to Assimp debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	// Loading FBX
	//LoadFBX("Assets/warrior.FBX"); 
	//LoadFBX("Assets/demon.fbx");
	//LoadFBX("Assets/BakerHouse.fbx");

	// Load textures
	CreateTexture();

	return true;
}

// Called before Update
update_status ModuleMeshLoader::PreUpdate(float dt)
{
	BROFILER_CATEGORY("MeshLoaderPreUpdate", Profiler::Color::Orange)

	return UPDATE_CONTINUE;
}

// Called every frame
update_status ModuleMeshLoader::Update(float dt)
{
	BROFILER_CATEGORY("MeshLoaderUpdate", Profiler::Color::LightSeaGreen)	
	
	for (int i = 0; i < FBX_list.size(); ++i)
		RenderFBX(FBX_list[i]);

	return UPDATE_CONTINUE;
}

// Called after Update
update_status ModuleMeshLoader::PostUpdate(float dt)
{
	BROFILER_CATEGORY("MeshLoaderPostUpdate", Profiler::Color::Yellow)
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleMeshLoader::CleanUp()
{
	LOG("Freeing mesh loader");
	// Detach Assimp log stream
	aiDetachAllLogStreams();

	for (int i = 0; i < FBX_list.size(); ++i)
		FBX_list[i].clear();

	FBX_list.clear();

	return true;
}

void ModuleMeshLoader::LoadFBX(const char* path) {

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
		aiReleaseImport(scene);
	}
	else LOG("Error loading scene %s", path);
}

void ModuleMeshLoader::RenderFBX(FBX_Mesh mesh) {
	
	for (int i = 0; i < mesh.size(); ++i) {

		glEnableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, test_tex_buffer);
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

void ModuleMeshLoader::CreateTexture()
{
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &test_tex_buffer);
	glBindTexture(GL_TEXTURE_2D, test_tex_buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleMeshLoader::Load(const json &config)
{
}

void ModuleMeshLoader::Save(json &config)
{
}
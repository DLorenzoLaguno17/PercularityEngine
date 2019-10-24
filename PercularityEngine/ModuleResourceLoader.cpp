#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "OpenGL.h"
#include "GameObject.h"
#include <string>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "DevIL/include/ilut.h"
#include "Brofiler/Lib/Brofiler.h"

#include "MathGeoLib/include/MathGeoLib.h"
#ifdef _DEBUG
#pragma comment( lib, "MathGeoLib/lib86/Debug/MathGeoLib.lib" )
#else
#pragma comment( lib, "MathGeoLib/lib86/Release/MathGeoLib.lib" )
#endif

#pragma comment (lib, "Assimp/lib86/assimp.lib")
#pragma comment (lib, "DevIL/lib86/DevIL.lib" )
#pragma comment (lib, "DevIL/lib86/ILU.lib" )
#pragma comment (lib, "DevIL/lib86/ILUT.lib" )

#include "mmgr/mmgr.h"

ModuleResourceLoader::ModuleResourceLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleResourceLoader::~ModuleResourceLoader()
{}

// Called before render is available
bool ModuleResourceLoader::Init()
{
	LOG("Preparing resource loader");

	// DevIL version checking
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION || iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) {
		LOG("DevIL version is different...exiting!\n");
		return 1;
	}

	// Initialize DevIL
	LOG("Initialazing DevIL");
	ilInit();
	iluInit();

	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);
	ilutRenderer(ILUT_OPENGL);

	ilutInit();

	return true;
}

// Called before the first frame
bool ModuleResourceLoader::Start()
{
	// Stream log messages to Assimp debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	// Load textures
	house_tex = CreateTexture("Assets/Textures/Bh.dds");
	demon_tex = CreateTexture("Assets/Textures/Difuse.png");
	icon_tex = CreateTexture("Assets/Textures/icon.png");

	// Loading FBX
	//LoadFBX("Assets/FBX/warrior.FBX"); 
	//LoadFBX("Assets/FBX/demon2.fbx", demon_tex); 
	LoadFBX("Assets/FBX/BakerHouse.fbx", house_tex);
	loadedAll = true;

	// Enable textures
	glEnable(GL_TEXTURE_2D);

	return true;
}

GameObject* ModuleResourceLoader::CreateGameObject() {
	GameObject* ret = nullptr;

	return ret;
}

// Called before quitting
bool ModuleResourceLoader::CleanUp()
{
	LOG("Freeing mesh loader");
	// Detach Assimp log stream
	aiDetachAllLogStreams();

	return true;
}

void ModuleResourceLoader::LoadFBX(const char* path, uint tex) {
	
	BROFILER_CATEGORY("ResourceLoaderLoadFBX", Profiler::Color::MediumVioletRed)
		
	std::string n = path;
	std::string directory;
	
	size_t i;
	if (loadedAll) i = n.rfind('\\', n.length());
	else i = n.rfind('//', n.length());

	if (i != std::string::npos) {
		directory = (n.substr(i + 1, n.length() - i));
	}

	std::string name;
	const size_t last_slash_idx = directory.rfind('.');
	if (std::string::npos != last_slash_idx)
	{
		name = directory.substr(0, last_slash_idx);
	}

	GameObject fbx_mesh(name);

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; ++i) {

			MeshData* m = new MeshData;

			// Copy vertices
			m->num_vertices = scene->mMeshes[i]->mNumVertices;
			m->vertices = new float[m->num_vertices * 3];
			memcpy(m->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * m->num_vertices * 3);
			LOG("New mesh with %d vertices", m->num_vertices);

			// Copy normals
			if (scene->mMeshes[i]->HasNormals())
			{
				m->num_normals = scene->mMeshes[i]->mNumVertices;
				m->normals = new float[m->num_normals * 3];
				memcpy(m->normals, scene->mMeshes[i]->mNormals, sizeof(float) * m->num_normals * 3);
			}

			// Copy colors
			if (scene->mMeshes[i]->HasVertexColors(0))
			{
				m->num_colors = scene->mMeshes[i]->mNumVertices;
				m->colors = new uint[m->num_colors * 4];

				for (uint j = 0; j < m->num_colors; ++j)
				{
					m->colors[j * 4] = scene->mMeshes[i]->mColors[0][j].r;
					m->colors[j * 4 + 1] = scene->mMeshes[i]->mColors[0][j].g;
					m->colors[j * 4 + 2] = scene->mMeshes[i]->mColors[0][j].b;
					m->colors[j * 4 + 3] = scene->mMeshes[i]->mColors[0][j].a;
				}
			}

			// Copy faces
			if (scene->mMeshes[i]->HasFaces())
			{
				m->num_indices = scene->mMeshes[i]->mNumFaces;
				m->indices = new uint[m->num_indices * 3]; // Assume each face is a triangle
				for (uint j = 0; j < m->num_indices; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
						LOG("WARNING, geometry face with != 3 indices!")
					else
						memcpy(&m->indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, sizeof(uint) * 3);
				}
			}

			// Copy texture UVs
			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				m->num_tex = scene->mMeshes[i]->mNumVertices;
				m->textures = new float[m->num_tex * 2];

				for (uint j = 0; j < m->num_tex; ++j)
				{
					m->textures[j * 2] = scene->mMeshes[i]->mTextureCoords[0][j].x;
					m->textures[j * 2 + 1] = scene->mMeshes[i]->mTextureCoords[0][j].y;
				}
			}

			// Assigning the VRAM
			glGenBuffers(1, (GLuint*)&m->id_vertex);
			glBindBuffer(GL_ARRAY_BUFFER, m->id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m->num_vertices, m->vertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, (GLuint*)&m->id_index);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 3 * m->num_indices, m->indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glGenBuffers(1, (GLuint*)&m->id_tex);
			glBindBuffer(GL_ARRAY_BUFFER, m->id_tex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m->num_tex, m->textures, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			fbx_mesh.c_mesh->mesh.push_back(m);
		}

		/*aiNode* node = scene->mRootNode;

		for (uint i = 0; i < node->mNumChildren; ++i) {			
			aiVector3D translation, scale;
			aiQuaternion rotation;
			node->mTransformation.Decompose(scale, rotation, translation);

			//float3 t(translation.x, translation.y, translation.z);
			//Quat r(rotation.x, rotation.y, rotation.z, rotation.w);
			//float3 s(scale.x, scale.y, scale.z);
			//fbx_mesh.transform = math::float4x4::FromTRS(t, r, s);
		}*/

		fbx_mesh.c_texture->texture = tex;
		App->scene->game_objects.push_back(fbx_mesh);
		LOG("Loaded new model %s. Current GameObjects on scene: %d", fbx_mesh.name.c_str(), App->scene->game_objects.size());
		LOG("Loaded new model %s. Current GameObjects on scene: %d", path, App->scene->game_objects.size());

		aiReleaseImport(scene);
	}
	else LOG("Error loading FBX: %s", path);
}

uint ModuleResourceLoader::CreateTexture(const char* path)
{
	ILuint image; 
	GLuint tex;
	ilGenImages(1, &image); 
	ilBindImage(image);

	if (!ilLoadImage(path)) {
		ilDeleteImages(1, &image);
		LOG("The texture image could not be loaded")
		return 0;
	}
	else {
		tex = ilutGLBindTexImage();
		LOG("Created texture from path: %s", path)

		long h, w, bpp, f;
		ILubyte *texdata = 0;

		w = ilGetInteger(IL_IMAGE_WIDTH);
		h = ilGetInteger(IL_IMAGE_HEIGHT);
		bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
		f = ilGetInteger(IL_IMAGE_FORMAT);
		texdata = ilGetData();

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, GL_MAX_TEXTURE_MAX_ANISOTROPY);
		gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, w, h, f, GL_UNSIGNED_BYTE, texdata);
		glBindTexture(GL_TEXTURE_2D, 0);

		ilBindImage(0);
		ilDeleteImage(image);

		return tex;
	}
}

void ModuleResourceLoader::Load(const json &config)
{
}

void ModuleResourceLoader::Save(json &config)
{
}
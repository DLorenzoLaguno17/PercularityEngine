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
	CreateDefaultTexture();
	icon_tex = CreateTexture("Assets/Textures/icon.png");

	// Loading FBX
	//LoadFBX("Assets/FBX/BakerHouse.fbx");
	loadedAll = true;

	// Enable textures
	glEnable(GL_TEXTURE_2D);

	return true;
}

// Called before quitting
bool ModuleResourceLoader::CleanUp()
{
	LOG("Freeing mesh loader");
	// Detach Assimp log stream
	aiDetachAllLogStreams();

	return true;
}

std::string ModuleResourceLoader::getNameFromPath(std::string path, bool withExtension) {
	std::string full_name;

	if (loadedAll) full_name = path.substr(path.find_last_of("\\") + 1);
	else full_name = path.substr(path.find_last_of("//") + 1);

	if (withExtension)
		return full_name;
	else {
		std::string::size_type const p(full_name.find_last_of('.'));
		std::string file_name = full_name.substr(0, p);

		return file_name;
	}
}

void ModuleResourceLoader::LoadFBX(const char* path, uint tex) {
	
	BROFILER_CATEGORY("ResourceLoaderLoadFBX", Profiler::Color::MediumVioletRed)
		
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; ++i) {

			GameObject* fbx_mesh= new GameObject(scene->mMeshes[i]->mName.C_Str());
			MeshData m;

			// Copy vertices
			m.num_vertices = scene->mMeshes[i]->mNumVertices;
			m.vertices = new float3[m.num_vertices];
			memcpy(m.vertices, scene->mMeshes[i]->mVertices, sizeof(float3) * m.num_vertices);
			LOG("NEW MESH");
			LOG("Vertices: %d", m.num_vertices);

			// Copy faces
			if (scene->mMeshes[i]->HasFaces())
			{
				m.num_indices = scene->mMeshes[i]->mNumFaces;
				m.indices = new uint[m.num_indices * 3]; // Assume each face is a triangle
				for (uint j = 0; j < m.num_indices; ++j)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
						LOG("WARNING, geometry face with != 3 indices!")
					else
						memcpy(&m.indices[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, sizeof(uint) * 3);
				}
				LOG("Faces: %d", m.num_indices / 3);
			}

			// Copy normals
			if (scene->mMeshes[i]->HasNormals())
			{
				m.normals = new float3[m.num_vertices];
				memcpy(m.normals, scene->mMeshes[i]->mNormals, sizeof(float3) * m.num_vertices);

				//Calculate the positions and vectors of the face Normals
				/*num_faces = loaded_mesh->mNumFaces;
				normals_faces = new float3[num_index];
				normals_faces_vector = new float3[num_index];
				for (int j = 0; j < num_index; j += 3)
				{
					// 3 points of the triangle/face
					float3 vert1 = vertex[index[j]];
					float3 vert2 = vertex[index[j + 1]];
					float3 vert3 = vertex[index[j + 2]];

					//Calculate starting point of the normal
					normals_faces[j] = (vert1 + vert2 + vert3) / 3;

					//Calculate Cross product of 2 edges of the triangle to obtain Normal vector
					float3 edge_a = vert2 - vert1;
					float3 edge_b = vert3 - vert1;

					float3 normal;
					normal = Cross(edge_a, edge_b);
					normal.Normalize();

					normals_faces_vector[j] = normal * 0.25f;
				}*/
			}

			// Copy colors
			if (scene->mMeshes[i]->HasVertexColors(0))
			{
				m.num_colors = scene->mMeshes[i]->mNumVertices;
				m.colors = new uint[m.num_colors * 4];

				for (uint j = 0; j < m.num_colors; ++j)
				{
					m.colors[j * 4] = scene->mMeshes[i]->mColors[0][j].r;
					m.colors[j * 4 + 1] = scene->mMeshes[i]->mColors[0][j].g;
					m.colors[j * 4 + 2] = scene->mMeshes[i]->mColors[0][j].b;
					m.colors[j * 4 + 3] = scene->mMeshes[i]->mColors[0][j].a;
				}
			}

			// Copy texture UVs
			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				m.num_tex = scene->mMeshes[i]->mNumVertices;
				m.textures = new float[m.num_tex * 2];

				for (uint j = 0; j < m.num_tex; ++j)
				{
					m.textures[j * 2] = scene->mMeshes[i]->mTextureCoords[0][j].x;
					m.textures[j * 2 + 1] = scene->mMeshes[i]->mTextureCoords[0][j].y;
				}
			}

			// Assigning the VRAM
			glGenBuffers(1, (GLuint*)&m.id_vertex);
			glBindBuffer(GL_ARRAY_BUFFER, m.id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * m.num_vertices, m.vertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, (GLuint*)&m.id_index);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 3 * m.num_indices, m.indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glGenBuffers(1, (GLuint*)&m.id_tex);
			glBindBuffer(GL_ARRAY_BUFFER, m.id_tex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m.num_tex, m.textures, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Copy materials
			aiMaterial* material = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];
			uint totalTex = material->GetTextureCount(aiTextureType_DIFFUSE);
			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

			std::string file = "Assets/Textures/";
			std::string name = getNameFromPath(path.C_Str(), true);
			std::string full_path = file + name;

			fbx_mesh->c_texture->texture = CreateTexture(full_path.c_str(), &fbx_mesh->c_texture->width, &fbx_mesh->c_texture->height);
			fbx_mesh->c_texture->tex_name = name;

			if (fbx_mesh->c_texture->texture == 0) {
				fbx_mesh->c_texture->texture = default_tex;
				fbx_mesh->c_texture->tex_name = "Default texture";
			}

			fbx_mesh->c_mesh->mesh = m;

		aiNode* node = scene->mRootNode;



		for (uint i = 0; i < node->mNumChildren; ++i) {			
			aiVector3D translation, scale;
			aiQuaternion rotation;
			node->mTransformation.Decompose(scale, rotation, translation);

			float3 t(translation.x, translation.y, translation.z);
			Quat r(rotation.x, rotation.y, rotation.z, rotation.w);
			float3 s(scale.x, scale.y, scale.z);
			fbx_mesh->c_transform->transform = math::float4x4::FromTRS(t, r, s);
		}
			App->scene->game_objects.push_back(fbx_mesh);
			LOG("Loaded new model %s. Current GameObjects on scene: %d", fbx_mesh->name.c_str(), App->scene->game_objects.size());
			LOG("_____________________");
		}
		aiReleaseImport(scene);
	}
	else LOG("Error loading FBX: %s", path);
}

uint ModuleResourceLoader::CreateTexture(const char* path, long* width, long* height)
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

		if (width) *width = w;
		if (height) *height = h;

		return tex;
	}
}

void ModuleResourceLoader::CreateDefaultTexture() {
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
	glGenTextures(1, &default_tex);
	glBindTexture(GL_TEXTURE_2D, default_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleResourceLoader::Load(const json &config)
{
}

void ModuleResourceLoader::Save(json &config)
{
}
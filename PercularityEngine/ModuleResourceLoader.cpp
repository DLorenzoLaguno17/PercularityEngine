#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
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
	icon_tex = new ComponentMaterial(COMPONENT_TYPE::MATERIAL, nullptr, true);
	icon_tex->CreateTexture("Assets/Textures/icon.png"); 

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

	if (App->scene->loadedAll) full_name = path.substr(path.find_last_of("\\") + 1);
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
			
			GameObject* fbx_mesh = new GameObject(scene->mMeshes[i]->mName.C_Str());
			fbx_mesh->MakeParent(App->scene->root);
			MeshData m;

			// Copy vertices
			m.num_vertices = scene->mMeshes[i]->mNumVertices;
			m.vertices = new float3[m.num_vertices];
			memcpy(m.vertices, scene->mMeshes[i]->mVertices, sizeof(float3) * m.num_vertices);
			LOG("NEW MESH")
			LOG("Vertices: %d", m.num_vertices)

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
				m.num_normals = scene->mMeshes[i]->mNumVertices;
				m.normals = new float3[m.num_normals];
				memcpy(m.normals, scene->mMeshes[i]->mNormals, sizeof(float3) * m.num_normals);
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
			aiString p;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &p);

			std::string file = "Assets/Textures/";
			std::string name = getNameFromPath(p.C_Str(), true);
			std::string full_path = file + name;
						
			ComponentMesh* mesh = fbx_mesh->GetComponent<ComponentMesh>();
			if (mesh == nullptr)
				mesh = (ComponentMesh*)fbx_mesh->CreateComponent(COMPONENT_TYPE::MESH, fbx_mesh);
			
			mesh->mesh = m;
			mesh->CreateBoundingBox();

			ComponentMaterial* objectMaterial = fbx_mesh->GetComponent<ComponentMaterial>();
			if (objectMaterial == nullptr)
				objectMaterial = (ComponentMaterial*)fbx_mesh->CreateComponent(COMPONENT_TYPE::MATERIAL, fbx_mesh);

			objectMaterial->CreateTexture(full_path.c_str());
			//m.CleanUp();

			aiNode* node = scene->mRootNode;

			for (uint i = 0; i < node->mNumChildren; ++i) {			
				aiVector3D translation, scale;
				aiQuaternion rotation;
				node->mTransformation.Decompose(scale, rotation, translation);

				float3 t(translation.x, translation.y, translation.z);
				Quat r(rotation.x, rotation.y, rotation.z, rotation.w);
				float3 s(scale.x, scale.y, scale.z);
				fbx_mesh->transform->transform = math::float4x4::FromTRS(t, r, s);
			}

			App->scene->game_objects.push_back(fbx_mesh);
			LOG("Loaded new model %s. Current GameObjects on scene: %d", fbx_mesh->name.c_str(), App->scene->game_objects.size());
			LOG("_____________________");
		}
		aiReleaseImport(scene);
	}
	else LOG("Error loading FBX: %s", path);
}

void ModuleResourceLoader::ImportFile(const char* full_path) {
	// We create the correct path
	std::string final_path;
	std::string extension;

	App->file_system->SplitFilePath(full_path, nullptr, &final_path, &extension);

	if (strcmp(extension.c_str(), "dds") == 0 || strcmp(extension.c_str(), "png") == 0)
		final_path = ASSETS_TEXTURE_FOLDER + final_path;
	if (strcmp(extension.c_str(), "fbx") == 0 || strcmp(extension.c_str(), "FBX") == 0)
		final_path = ASSETS_MODEL_FOLDER + final_path;

	// We try to copy the file to its corresponding assets' folder
	if (App->file_system->CopyFromOutsideFS(full_path, final_path.c_str()))
	{
		// If it has been copied, we import it to our own libraries
		std::string written_file;
		if (strcmp(extension.c_str(), "dds") == 0 || strcmp(extension.c_str(), "png") == 0)
		{
			ImportTexture(final_path.c_str(), written_file);
		}
		else if (strcmp(extension.c_str(), "fbx") == 0 || strcmp(extension.c_str(), "FBX") == 0) {
			//ComponentMesh* mesh = new ComponentMesh(COMPONENT_TYPE::MESH, nullptr, true);
			//ImportMesh(mesh, written_file);
		}
		else
			LOG("Importing of [%s] FAILED", final_path.c_str())
	}
}

bool ModuleResourceLoader::ImportMesh(ComponentMesh* mesh, std::string& output_file) {
	
	LOG("Importing mesh")
	bool ret = false; 

	// Amount of: 1.Indices / 2.Vertices / 3.Colors / 4.Normals / 5.UVs / 6.AABB
	uint ranges[5] = { 
		mesh->mesh.num_indices, 
		mesh->mesh.num_vertices, 
		mesh->mesh.num_colors, 
		mesh->mesh.num_normals, 
		mesh->mesh.num_tex 
	};

	uint size = sizeof(ranges) 
		+ sizeof(uint) * mesh->mesh.num_indices * 3
		+ sizeof(float3) * mesh->mesh.num_vertices
		+ sizeof(uint) * mesh->mesh.num_colors * 4
		+ sizeof(float3) * mesh->mesh.num_normals
		+ sizeof(float) * mesh->mesh.num_tex * 2;
	
	// Allocate memory
	char* data = new char[size]; 
	char* cursor = data;

	// First store ranges
	uint bytes = sizeof(ranges); 
	memcpy(cursor, ranges, bytes);

	// Store indices
	cursor += bytes; 
	bytes = sizeof(uint) * mesh->mesh.num_indices * 3;
	memcpy(cursor, mesh->mesh.indices, bytes);

	// Store vertices
	cursor += bytes;
	bytes = sizeof(float3) * mesh->mesh.num_vertices;
	memcpy(cursor, mesh->mesh.vertices, bytes);

	// Store colors
	cursor += bytes;
	bytes = sizeof(uint) * mesh->mesh.num_colors * 4;
	memcpy(cursor, mesh->mesh.colors, bytes);

	// Store normals
	cursor += bytes;
	bytes = sizeof(float3) * mesh->mesh.num_normals;
	memcpy(cursor, mesh->mesh.normals, bytes);

	// Store UVs
	cursor += bytes;
	bytes = sizeof(float) * mesh->mesh.num_tex * 2;
	memcpy(cursor, mesh->mesh.textures, bytes);

	ret = App->file_system->SaveUnique(output_file, cursor, size, LIBRARY_MESH_FOLDER, "mesh", "fbx");
	RELEASE_ARRAY(cursor);

	if (ret) LOG("Imported mesh: %s", output_file)
	else LOG("Could not import mesh")

	return ret;
}

void ModuleResourceLoader::LoadMesh(ComponentMesh* mesh, char* buffer) {
	
	LOG("Loading mesh")
	char* cursor = buffer;

	// Amount of: 1.Indices / 2.Vertices / 3.Colors / 4.Normals / 5.UVs / 6.AABB
	uint ranges[5];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);

	mesh->mesh.num_indices = ranges[0];
	mesh->mesh.num_vertices = ranges[1];
	mesh->mesh.num_colors = ranges[2];
	mesh->mesh.num_normals = ranges[3];
	mesh->mesh.num_tex = ranges[4];

	// Load indices
	cursor += bytes;
	bytes = sizeof(uint) * mesh->mesh.num_indices * 3;
	mesh->mesh.indices = new uint[mesh->mesh.num_indices];
	memcpy(mesh->mesh.indices, cursor, bytes);

	// Load vertices
	cursor += bytes;
	bytes = sizeof(float3) * mesh->mesh.num_vertices;
	mesh->mesh.vertices = new float3[mesh->mesh.num_vertices];
	memcpy(mesh->mesh.vertices, cursor, bytes);

	// Load colors
	cursor += bytes;
	bytes = sizeof(uint) * mesh->mesh.num_colors * 4;
	mesh->mesh.colors = new uint[mesh->mesh.num_colors];
	memcpy(mesh->mesh.colors, cursor, bytes);

	// Load normals
	cursor += bytes;
	bytes = sizeof(float3) * mesh->mesh.num_normals;
	mesh->mesh.normals = new float3[mesh->mesh.num_normals];
	memcpy(mesh->mesh.normals, cursor, bytes);

	// Load UVs
	cursor += bytes;
	bytes = sizeof(float) * mesh->mesh.num_tex;
	mesh->mesh.textures = new float[mesh->mesh.num_tex];
	memcpy(mesh->mesh.textures, cursor, bytes);
	LOG("Loaded mesh")
}

bool ModuleResourceLoader::ImportTexture(const char* path, std::string& output_file) {
	bool ret = false;

	ILuint size;
	ILubyte *data;

	// We pick a specific DXT compression use and get the size of the data buffer
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, NULL, 0);

	if (size > 0) {
		// We allocate data buffer
		data = new ILubyte[size];
		// Save to buffer
		if (ilSaveL(IL_DDS, data, size) > 0)
			ret = App->file_system->SaveUnique(output_file, data, size, LIBRARY_TEXTURE_FOLDER, "texture", "dds");
		RELEASE_ARRAY(data);
	}

	if (!ret) LOG("Cannot load texture %s from path %s", path);

	return ret;
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
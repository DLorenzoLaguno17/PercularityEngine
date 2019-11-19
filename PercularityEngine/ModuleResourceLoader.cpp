#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "OpenGL.h"

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
	icon_tex = new ComponentMaterial(nullptr, true);
	LoadTexture("Assets/Textures/icon.png", icon_tex); 

	// Enable textures
	glEnable(GL_TEXTURE_2D);

	return true;
}

// Called before quitting
bool ModuleResourceLoader::CleanUp()
{
	LOG("Freeing resource loader");
	// Detach Assimp log stream
	aiDetachAllLogStreams();
	RELEASE(icon_tex);

	return true;
}

void ModuleResourceLoader::ImportFile(const char* full_path) {
	// We create the correct path
	std::string final_path;
	std::string extension;

	App->file_system->SplitFilePath(full_path, nullptr, &final_path, &extension);

	if (CheckTextureExtension(extension.c_str()))
		final_path = ASSETS_TEXTURE_FOLDER + final_path;
	if (CheckMeshExtension(extension.c_str()))
		final_path = ASSETS_MODEL_FOLDER + final_path;

	// We try to copy the file to its corresponding assets' folder
	if (App->file_system->CopyFromOutsideFS(full_path, final_path.c_str()))
	{
		// If it has been copied, we import it to our own libraries
		std::string written_file;
		if (CheckTextureExtension(extension.c_str()))
		{
			// If the path has texture extensions, it calls the LoadTexture() method
			ComponentMaterial* material = App->scene->selected->GetComponent<ComponentMaterial>();
			if (!material) material = (ComponentMaterial*)App->scene->selected->CreateComponent(COMPONENT_TYPE::MATERIAL);

			LoadTexture(full_path, material);
			material = nullptr;
		}
		else if (CheckMeshExtension(extension.c_str())) {
			// If the path has texture extensions, it calls the LoadModel() method
			ComponentMesh* mesh = new ComponentMesh(nullptr, true);

			LoadModel(full_path);
		}
		else
			LOG("Importing of [%s] FAILED", final_path.c_str())
	}
}

void ModuleResourceLoader::LoadModel(const char* path) {
	
	BROFILER_CATEGORY("ResourceLoaderLoadFBX", Profiler::Color::MediumVioletRed)
		
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		GameObject* parent_mesh = new GameObject(getNameFromPath(path), App->scene->GetRoot());

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; ++i) {
			
			// Create the GameObject
			GameObject* fbx_mesh = nullptr;
			if (scene->mNumMeshes > 1) 
				fbx_mesh = new GameObject(scene->mMeshes[i]->mName.C_Str(), parent_mesh);
			else 
				fbx_mesh = parent_mesh;

			// Copy the mesh						
			ComponentMesh* mesh = (ComponentMesh*)fbx_mesh->CreateComponent(COMPONENT_TYPE::MESH);
			LoadMesh(mesh, scene->mMeshes[i]);
			
			// Copy materials
			aiMaterial* aux_mat = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];
			uint totalTex = aux_mat->GetTextureCount(aiTextureType_DIFFUSE);
			aiString p;
			aux_mat->GetTexture(aiTextureType_DIFFUSE, 0, &p);

			std::string file = "Assets/Textures/";
			std::string name = getNameFromPath(p.C_Str(), true);
			std::string full_path = file + name;

			ComponentMaterial* material = (ComponentMaterial*)fbx_mesh->CreateComponent(COMPONENT_TYPE::MATERIAL);
			LoadTexture(full_path.c_str(), material);

			App->scene->selected = fbx_mesh;
			App->scene->numGameObjectsInScene++;

			LOG("Loaded new model %s.", fbx_mesh->name.c_str());
			LOG("_____________________");
		}
		aiReleaseImport(scene);
	}
	else LOG("Error loading FBX: %s", path);
}

// -----------------------------------------------------------------------------------------------
// MESH-RELATED METHODS
// -----------------------------------------------------------------------------------------------

void ModuleResourceLoader::LoadMesh(ComponentMesh* c_mesh, aiMesh* currentMesh) {
	MeshData m;

	// Copy vertices
	m.num_vertices = currentMesh->mNumVertices;
	m.vertices = new float3[m.num_vertices];
	memcpy(m.vertices, currentMesh->mVertices, sizeof(float3) * m.num_vertices);
	
	LOG("NEW MESH");
	LOG("Vertices: %d", m.num_vertices);

	// Copy faces
	if (currentMesh->HasFaces())
	{
		m.num_indices = currentMesh->mNumFaces * 3;
		m.indices = new uint[m.num_indices];

		for (unsigned j = 0; j < currentMesh->mNumFaces; ++j)
		{
			const aiFace& face = currentMesh->mFaces[j];

			// Only triangles
			if (face.mNumIndices > 3)
			{
				LOG("Importer Mesh found a quad in %s, ignoring it. ", currentMesh->mName);
				continue;
			}

			m.indices[j * 3] = face.mIndices[0];
			m.indices[j * 3 + 1] = face.mIndices[1];
			m.indices[j * 3 + 2] = face.mIndices[2];
		}
		LOG("Faces: %d", m.num_indices / 3);
	}

	// Copy normals
	if (currentMesh->HasNormals())
	{
		m.num_normals = currentMesh->mNumVertices;
		m.normals = new float3[m.num_normals];
		memcpy(m.normals, currentMesh->mNormals, sizeof(float3) * m.num_normals);
	}

	// Copy colors
	if (currentMesh->HasVertexColors(0))
	{
		m.num_colors = currentMesh->mNumVertices * 4;
		m.colors = new uint[m.num_colors];

		for (uint i = 0; i < currentMesh->mNumVertices; ++i)
		{
			m.colors[i * 4] = currentMesh->mColors[0][i].r;
			m.colors[i * 4 + 1] = currentMesh->mColors[0][i].g;
			m.colors[i * 4 + 2] = currentMesh->mColors[0][i].b;
			m.colors[i * 4 + 3] = currentMesh->mColors[0][i].a;
		}
	}

	// Copy texture UVs
	if (currentMesh->HasTextureCoords(0))
	{
		m.num_UVs = currentMesh->mNumVertices * 2;
		m.coords = new float[m.num_UVs]; 

		for (uint i = 0; i < currentMesh->mNumVertices; ++i)
		{
			m.coords[i * 2] = currentMesh->mTextureCoords[0][i].x;
			m.coords[i * 2 + 1] = currentMesh->mTextureCoords[0][i].y;
		}
	}

	// Assigning the VRAM
	m.id_vertex = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float3) * m.num_vertices, m.vertices);
	m.id_index = App->renderer3D->CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m.num_indices, m.indices);
	m.id_UVs = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float) * m.num_UVs, m.coords);

	// Assigning the MeshData to the ComponentMesh and creating the AABB
	c_mesh->mesh = m;
	c_mesh->aabb.SetNegativeInfinity();
	c_mesh->aabb = AABB::MinimalEnclosingAABB(m.vertices, m.num_vertices);
	c_mesh->mesh_name = c_mesh->gameObject->name;
	
	// We import the mesh to our library
	std::string path = LIBRARY_MESH_FOLDER + c_mesh->gameObject->name + ".mesh";
	ImportMeshToLibrary(path.c_str(), c_mesh);
}

bool ModuleResourceLoader::ImportMeshToLibrary(const char* path, ComponentMesh* mesh) {
	
	LOG("Importing mesh");
	bool ret = false; 

	// Amount of: 1.Indices / 2.Vertices / 3.Colors / 4.Normals / 5.UVs
	uint ranges[5] = { 
		mesh->mesh.num_indices, 
		mesh->mesh.num_vertices, 
		mesh->mesh.num_colors, 
		mesh->mesh.num_normals, 
		mesh->mesh.num_UVs 
	};

	// We allocate data buffer
	uint size = sizeof(ranges) 
		+ sizeof(uint) * mesh->mesh.num_indices
		+ sizeof(float3) * mesh->mesh.num_vertices
		+ sizeof(uint) * mesh->mesh.num_colors
		+ sizeof(float3) * mesh->mesh.num_normals
		+ sizeof(float) * mesh->mesh.num_UVs;

	if (size > 0) {
		// Allocate memory
		char* data = new char[size];
		char* cursor = data;

		// First store ranges
		uint bytes = sizeof(ranges);
		memcpy(cursor, ranges, bytes);

		// Store indices
		cursor += bytes;
		bytes = sizeof(uint) * mesh->mesh.num_indices;
		memcpy(cursor, mesh->mesh.indices, bytes);

		// Store vertices
		cursor += bytes;
		bytes = sizeof(float3) * mesh->mesh.num_vertices;
		memcpy(cursor, mesh->mesh.vertices, bytes);

		// Store colors
		cursor += bytes;
		bytes = sizeof(uint) * mesh->mesh.num_colors;
		memcpy(cursor, mesh->mesh.colors, bytes);

		// Store normals
		cursor += bytes;
		bytes = sizeof(float3) * mesh->mesh.num_normals;
		memcpy(cursor, mesh->mesh.normals, bytes);

		// Store UVs
		cursor += bytes;
		bytes = sizeof(float) * mesh->mesh.num_UVs;
		memcpy(cursor, mesh->mesh.coords, bytes);

		ret = App->file_system->Save(path, data, size);		
		
		RELEASE_ARRAY(data);
		cursor = nullptr;

		LOG("Imported mesh: %s", getNameFromPath(path).c_str());
	}

	if (!ret) LOG("Could not import mesh");

	return ret;
}

void ModuleResourceLoader::LoadMeshFromLibrary(const char* path, ComponentMesh* mesh) {
	
	char* buffer = nullptr;
	App->file_system->Load(path, &buffer);
	
	if (buffer) {
		char* cursor = buffer;

		// Amount of: 1.Indices / 2.Vertices / 3.Colors / 4.Normals / 5.UVs
		uint ranges[5];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		mesh->mesh.num_indices = ranges[0];
		mesh->mesh.num_vertices = ranges[1];
		mesh->mesh.num_colors = ranges[2];
		mesh->mesh.num_normals = ranges[3];
		mesh->mesh.num_UVs = ranges[4];

		// Load indices
		cursor += bytes;
		bytes = sizeof(uint) * mesh->mesh.num_indices;
		mesh->mesh.indices = new uint[mesh->mesh.num_indices];
		memcpy(mesh->mesh.indices, cursor, bytes);

		// Load vertices
		cursor += bytes;
		bytes = sizeof(float3) * mesh->mesh.num_vertices;
		mesh->mesh.vertices = new float3[mesh->mesh.num_vertices];
		memcpy(mesh->mesh.vertices, cursor, bytes);

		// Load colors
		cursor += bytes;
		bytes = sizeof(uint) * mesh->mesh.num_colors;
		mesh->mesh.colors = new uint[mesh->mesh.num_colors];
		memcpy(mesh->mesh.colors, cursor, bytes);

		// Load normals
		cursor += bytes;
		bytes = sizeof(float3) * mesh->mesh.num_normals;
		mesh->mesh.normals = new float3[mesh->mesh.num_normals];
		memcpy(mesh->mesh.normals, cursor, bytes);

		// Load UVs
		cursor += bytes;
		bytes = sizeof(float) * mesh->mesh.num_UVs;
		mesh->mesh.coords = new float[mesh->mesh.num_UVs];
		memcpy(mesh->mesh.coords, cursor, bytes);

		// Assigning the VRAM
		glGenBuffers(1, (GLuint*)&mesh->mesh.id_vertex);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->mesh.id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * mesh->mesh.num_vertices, mesh->mesh.vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, (GLuint*)&mesh->mesh.id_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mesh.id_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->mesh.num_indices, mesh->mesh.indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenBuffers(1, (GLuint*)&mesh->mesh.id_UVs);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->mesh.id_UVs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->mesh.num_UVs, mesh->mesh.coords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		mesh->aabb.SetNegativeInfinity();
		mesh->aabb = AABB::MinimalEnclosingAABB(mesh->mesh.vertices, mesh->mesh.num_vertices);

		RELEASE_ARRAY(buffer);
		cursor = nullptr;
		LOG("Loaded mesh from library");
	}
}

// -----------------------------------------------------------------------------------------------
// TEXTURE-RELATED METHODS
// -----------------------------------------------------------------------------------------------

void ModuleResourceLoader::LoadTexture(const char* path, ComponentMaterial* material) {
	ILuint image;
	GLuint tex;
	ilGenImages(1, &image);
	ilBindImage(image);

	if (!ilLoadImage(path)) {
		ilDeleteImages(1, &image);
		LOG("The texture image could not be loaded");

		if (material) {
			material->tex_name = "Default texture";
			material->texture = default_tex;
		}
	}
	else {
		tex = ilutGLBindTexImage();
		LOG("Created texture from path: %s", path);
		LOG("");

		if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
			LOG("Error converting image: %s", iluErrorString(ilGetError()));

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

		// Storing texture data
		if (material) {
			material->width = w;
			material->height = h;
			material->tex_name = getNameFromPath(path, true);
			material->texture = tex;
		}
		
		// We import the texture to our library
		std::string name;
		ImportTextureToLibrary(path, name);

		ilBindImage(0);
		ilDeleteImage(image);
	}
}

// Stores a texture as a DDS in the library
bool ModuleResourceLoader::ImportTextureToLibrary(const char* path, std::string& output_file) {
	bool ret = false;

	ILuint size;
	ILubyte* data;

	// We pick a specific DXT compression use and get the size of the data buffer
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, NULL, 0);

	if (size > 0) {
		// We allocate data buffer
		data = new ILubyte[size];
		// Save to buffer
		if (ilSaveL(IL_DDS, data, size) > 0)
			ret = App->file_system->SaveUnique(output_file, data, size, LIBRARY_TEXTURE_FOLDER, getNameFromPath(path).c_str(), "dds");
		RELEASE_ARRAY(data);

		LOG("Imported texture %s", output_file);		
	}

	if (!ret) LOG("Cannot import texture from path %s", path);

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

// -----------------------------------------------------------------------------------------------
// OTHER METHODS
// -----------------------------------------------------------------------------------------------
std::string ModuleResourceLoader::getNameFromPath(std::string path, bool withExtension) {
	std::string full_name;

	App->file_system->NormalizePath(path);
	full_name = path.substr(path.find_last_of("//") + 1);

	if (withExtension)
		return full_name;
	else {
		std::string::size_type const p(full_name.find_last_of('.'));
		std::string file_name = full_name.substr(0, p);

		return file_name;
	}
}

// Methods to check the extension of a file
bool ModuleResourceLoader::CheckTextureExtension(const char* extension) {
	return (strcmp(extension, "dds") == 0 || strcmp(extension, "png") == 0 || strcmp(extension, "jpg") == 0);
}

bool ModuleResourceLoader::CheckMeshExtension(const char* extension) {
	return (strcmp(extension, "fbx") == 0 || strcmp(extension, "FBX") == 0);
}
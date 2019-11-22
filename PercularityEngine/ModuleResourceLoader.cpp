#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceScene.h"
#include "ResourceModel.h"
#include "GameObject.h"
#include "OpenGL.h"

#include <fstream>
#include <iomanip>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "DevIL/include/ilut.h"
#include "Brofiler/Lib/Brofiler.h"
#include "mmgr/mmgr.h"

#ifdef _DEBUG
#pragma comment( lib, "MathGeoLib/lib86/Debug/MathGeoLib.lib" )
#else
#pragma comment( lib, "MathGeoLib/lib86/Release/MathGeoLib.lib" )
#endif

#pragma comment (lib, "Assimp/lib86/assimp.lib")
#pragma comment (lib, "DevIL/lib86/DevIL.lib" )
#pragma comment (lib, "DevIL/lib86/ILU.lib" )
#pragma comment (lib, "DevIL/lib86/ILUT.lib" )

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

	modelAddress = "Library/Models/";

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
	CreateDefaultMaterial();
	LoadEngineUI();

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

	//RELEASE(icon_tex);
	//RELEASE(default_material);

	return true;
}

bool ModuleResourceLoader::LoadModel(const char* path, std::string& output_file, std::vector<ResourceMesh*>& meshes) {
	
	BROFILER_CATEGORY("ResourceLoaderLoadModel", Profiler::Color::MediumVioletRed)

	bool ret = false;
	
	// We adapt the  path for Assimp
	const char* adapted_path = strstr(path, "Assets");
		
	const aiScene* scene = aiImportFile(adapted_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* root_node = scene->mRootNode;

		// Use scene->mNumMeshes to iterate on scene->mMeshes array		
		if (root_node->mNumChildren > 0) {
			for (uint i = 0; i < root_node->mNumChildren; ++i){
				std::string output;
				ret = LoadNode(scene, root_node->mChildren[i], output, meshes, path);
			}
		}

		loaded_node = 0;
		aiReleaseImport(scene);
		output_file = "Model";
	}
	else LOG("Error loading FBX: %s", path);

	return ret;
}

// -----------------------------------------------------------------------------------------------
// MESH-RELATED METHODS
// -----------------------------------------------------------------------------------------------

bool ModuleResourceLoader::LoadNode(const aiScene* scene, aiNode* node, std::string& output_file, std::vector<ResourceMesh*>& meshes, const char* path) {	

	bool ret = false;

	aiVector3D translation;
	aiVector3D scale;
	aiQuaternion rotation;

	// We take the transform
	node->mTransformation.Decompose(scale, rotation, translation);
	float3 pos(translation.x, translation.y, translation.z);
	float3 sca(1, 1, 1);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	std::string nodeName = node->mName.C_Str();

	bool found = true;
	while (found) // Skipp all dummy modules. Assimp loads this fbx nodes to stack all transformations
	{
		// All dummy modules have one children (next dummy module or last module containing the mesh)
		if (nodeName.find("_$AssimpFbx$_") != std::string::npos && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next GameObject
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(scale, rotation, translation);
			pos += float3(translation.x, translation.y, translation.z);
			sca = float3(sca.x * scale.x, sca.y * scale.y, sca.z * scale.z);
			rot = rot * Quat(rotation.x, rotation.y, rotation.z, rotation.w);

			nodeName = node->mName.C_Str();

			/* If we find a dummy node we "change" our current node into the dummy one and search
			for other dummy nodes inside that one.*/
			found = true;
		}
		else found = false;
	}
	
	// -----------------------------------------------------------------------------------------------
	// WE START COPYING THE DATA
	// -----------------------------------------------------------------------------------------------	
	std::string output;
	ResourceMesh* parent_mesh = (ResourceMesh*)App->res_manager->CreateNewResource(RESOURCE_TYPE::MESH);
	parent_mesh->file = path;
	parent_mesh->name = nodeName;
	parent_mesh->position = pos;
	parent_mesh->scale = sca;
	parent_mesh->rotation = rot;

	bool hasMeshes = false;

	for (int i = 0; i < node->mNumMeshes; ++i) {		

		hasMeshes = true;

		ResourceMesh* mesh = nullptr;
		if (node->mNumMeshes > 1)
		{
			mesh = (ResourceMesh*)App->res_manager->CreateNewResource(RESOURCE_TYPE::MESH);
			mesh->file = path;
			mesh->name = nodeName;
			mesh->position = pos;
			mesh->scale = sca;
			mesh->rotation = rot;
		}
		else mesh = parent_mesh;

		mesh->renderizable = true;

		aiMesh* currentMesh = scene->mMeshes[node->mMeshes[i]];				

		// Check if it has any assigned texture
		aiMaterial* mat = scene->mMaterials[currentMesh->mMaterialIndex];
		aiString p;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &p);
		std::string n = getNameFromPath(p.C_Str(), true);
		mesh->assignedTex = n;

		// Copy vertices
		mesh->num_vertices = currentMesh->mNumVertices;
		mesh->vertices = new float3[mesh->num_vertices];
		memcpy(mesh->vertices, currentMesh->mVertices, sizeof(float3) * mesh->num_vertices);

		LOG("\nNEW MESH");
		LOG("Vertices: %d", mesh->num_vertices);

		// Copy faces
		bool correctFace = true;
		if (currentMesh->HasFaces())
		{
			mesh->num_indices = currentMesh->mNumFaces * 3;
			mesh->indices = new uint[mesh->num_indices];

			for (unsigned j = 0; j < currentMesh->mNumFaces; ++j)
			{
				const aiFace& face = currentMesh->mFaces[j];

				// Only triangles
				if (face.mNumIndices > 3)
				{
					LOG("Importer Mesh found a quad in %s, ignoring it. ", currentMesh->mName);
					correctFace = false;
					continue;
				}

				mesh->indices[j * 3] = face.mIndices[0];
				mesh->indices[j * 3 + 1] = face.mIndices[1];
				mesh->indices[j * 3 + 2] = face.mIndices[2];
			}
			LOG("Faces: %d", mesh->num_indices / 3);
		}

		// Copy normals
		if (currentMesh->HasNormals() && correctFace)
		{
			mesh->num_normals = currentMesh->mNumVertices;
			mesh->normals = new float3[mesh->num_normals];
			memcpy(mesh->normals, currentMesh->mNormals, sizeof(float3) * mesh->num_normals);
		}

		// Copy colors
		if (currentMesh->HasVertexColors(0))
		{
			mesh->num_colors = currentMesh->mNumVertices * 4;
			mesh->colors = new uint[mesh->num_colors];

			for (uint i = 0; i < currentMesh->mNumVertices; ++i)
			{
				mesh->colors[i * 4] = currentMesh->mColors[0][i].r;
				mesh->colors[i * 4 + 1] = currentMesh->mColors[0][i].g;
				mesh->colors[i * 4 + 2] = currentMesh->mColors[0][i].b;
				mesh->colors[i * 4 + 3] = currentMesh->mColors[0][i].a;
			}
		}

		// Copy texture UVs
		if (currentMesh->HasTextureCoords(0))
		{
			mesh->num_UVs = currentMesh->mNumVertices * 2;
			mesh->coords = new float[mesh->num_UVs];

			for (uint i = 0; i < currentMesh->mNumVertices; ++i)
			{
				mesh->coords[i * 2] = currentMesh->mTextureCoords[0][i].x;
				mesh->coords[i * 2 + 1] = currentMesh->mTextureCoords[0][i].y;
			}
		}

		// Assigning the VRAM
		mesh->id_vertex = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float3) * mesh->num_vertices, mesh->vertices);
		mesh->id_index = App->renderer3D->CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, mesh->indices);
		mesh->id_UVs = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_UVs, mesh->coords);

		// We import the mesh to our library
		char name[35];
		sprintf_s(name, 35, "%s %d", getNameFromPath(path).c_str(), loaded_node);
		std::string asdf = nodeName;
		ret = ImportMeshToLibrary(mesh, output_file, name);
		mesh->exported_file = output_file;
		loaded_node++;

		// We add it to the list of the meshes of the model resource
		meshes.push_back(mesh);
	}

	if (!hasMeshes) {
		char name[35];
		sprintf_s(name, 35, "%s %d", getNameFromPath(path).c_str(), loaded_node);
		std::string asdf = nodeName;
		ret = ImportMeshToLibrary(parent_mesh, output_file, name);
		parent_mesh->exported_file = output_file;
		loaded_node++;

		// We add it to the list of the meshes of the model resource
		meshes.push_back(parent_mesh);
	}

	if (node->mNumChildren > 0) {
		for (int i = 0; i < node->mNumChildren; ++i) {
			LoadNode(scene, node->mChildren[i], output_file, meshes, path);
		}
	}

	return ret;
}

bool ModuleResourceLoader::ImportMeshToLibrary(ResourceMesh* mesh, std::string& output_file, const char* name) {
	
	LOG("Importing mesh");
	bool ret = false; 

	// Amount of: 1.Indices / 2.Vertices / 3.Colors / 4.Normals / 5.UVs
	uint ranges[5] = { 
		mesh->num_indices, 
		mesh->num_vertices, 
		mesh->num_colors, 
		mesh->num_normals, 
		mesh->num_UVs 
	};

	// We allocate data buffer
	uint size = sizeof(ranges) 
		+ sizeof(uint) * mesh->num_indices
		+ sizeof(float3) * mesh->num_vertices
		+ sizeof(uint) * mesh->num_colors
		+ sizeof(float3) * mesh->num_normals
		+ sizeof(float) * mesh->num_UVs;

	if (size > 0) {
		// Allocate memory
		char* data = new char[size];
		char* cursor = data;

		// First store ranges
		uint bytes = sizeof(ranges);
		memcpy(cursor, ranges, bytes);

		// Store indices
		cursor += bytes;
		bytes = sizeof(uint) * mesh->num_indices;
		memcpy(cursor, mesh->indices, bytes);

		// Store vertices
		cursor += bytes;
		bytes = sizeof(float3) * mesh->num_vertices;
		memcpy(cursor, mesh->vertices, bytes);

		// Store colors
		cursor += bytes;
		bytes = sizeof(uint) * mesh->num_colors;
		memcpy(cursor, mesh->colors, bytes);

		// Store normals
		cursor += bytes;
		bytes = sizeof(float3) * mesh->num_normals;
		memcpy(cursor, mesh->normals, bytes);

		// Store UVs
		cursor += bytes;
		bytes = sizeof(float) * mesh->num_UVs;
		memcpy(cursor, mesh->coords, bytes);

		// Save the file
		ret = App->file_system->SaveUnique(output_file, data, size, LIBRARY_MESH_FOLDER, name, "mesh");

		RELEASE_ARRAY(data);
		cursor = nullptr;

		//LOG("Imported mesh: %s", getNameFromPath(path).c_str());
	}

	if (!ret) LOG("Could not import mesh");

	return ret;
}

bool ModuleResourceLoader::LoadMeshFromLibrary(ResourceMesh* mesh) {	
	
	bool ret = false;
	char* buffer = nullptr;
	App->file_system->Load(mesh->exported_file.c_str(), &buffer);

	if (buffer) {
		char* cursor = buffer;

		// Amount of: 1.Indices / 2.Vertices / 3.Colors / 4.Normals / 5.UVs
		uint ranges[5];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		mesh->num_indices = ranges[0];
		mesh->num_vertices = ranges[1];
		mesh->num_colors = ranges[2];
		mesh->num_normals = ranges[3];
		mesh->num_UVs = ranges[4];

		// Load indices
		cursor += bytes;
		bytes = sizeof(uint) * mesh->num_indices;
		mesh->indices = new uint[mesh->num_indices];
		memcpy(mesh->indices, cursor, bytes);

		// Load vertices
		cursor += bytes;
		bytes = sizeof(float3) * mesh->num_vertices;
		mesh->vertices = new float3[mesh->num_vertices];
		memcpy(mesh->vertices, cursor, bytes);

		// Load colors
		cursor += bytes;
		bytes = sizeof(uint) * mesh->num_colors;
		mesh->colors = new uint[mesh->num_colors];
		memcpy(mesh->colors, cursor, bytes);

		// Load normals
		cursor += bytes;
		bytes = sizeof(float3) * mesh->num_normals;
		mesh->normals = new float3[mesh->num_normals];
		memcpy(mesh->normals, cursor, bytes);

		// Load UVs
		cursor += bytes;
		bytes = sizeof(float) * mesh->num_UVs;
		mesh->coords = new float[mesh->num_UVs];
		memcpy(mesh->coords, cursor, bytes);

		// Assigning the VRAM
		mesh->id_vertex = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float3) * mesh->num_vertices, mesh->vertices);
		mesh->id_index = App->renderer3D->CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, mesh->indices);
		mesh->id_UVs = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_UVs, mesh->coords);

		// Creating the AABB
		mesh->aabb.SetNegativeInfinity();
		mesh->aabb = AABB::MinimalEnclosingAABB(mesh->vertices, mesh->num_vertices);

		ret = true;

		RELEASE_ARRAY(buffer);
		cursor = nullptr;
		LOG("------------------------");
		LOG("Loaded mesh from library");
	}

	return ret;
}

// -----------------------------------------------------------------------------------------------
// TEXTURE-RELATED METHODS
// -----------------------------------------------------------------------------------------------

bool ModuleResourceLoader::LoadTexture(const char* path, std::string& output_file) {
	bool ret = false;
	
	ILuint image;
	GLuint tex;
	ilGenImages(1, &image);
	ilBindImage(image);

	// We adapt the  path for DevIL
	const char* adapted_path = strstr(path, "Assets");

	if (!ilLoadImage(adapted_path)) {
		ilDeleteImages(1, &image);
		LOG("The texture image could not be loaded");		
	}
	else {
		ProcessTexture(tex);

		// We import the texture to our library
		if(ImportTextureToLibrary(path, output_file)) ret = true;

		ilBindImage(0);
		ilDeleteImage(image);
	}

	return ret;
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

bool ModuleResourceLoader::LoadTextureFromLibrary(ResourceTexture* tex) {
	bool ret = false;

	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);

	// We adapt the  path for DevIL
	const char* adapted_path = strstr(tex->exported_file.c_str(), "library");

	if (!ilLoadImage(adapted_path)) {
		ilDeleteImages(1, &image);
		LOG("The texture image could not be loaded");
	}
	else {
		ProcessTexture(tex->texture);
		LOG("");
		LOG("Created texture from path: %s", tex->exported_file.c_str());

		uint h, w;
		w = ilGetInteger(IL_IMAGE_WIDTH);
		h = ilGetInteger(IL_IMAGE_HEIGHT);

		tex->width = w;
		tex->height = h;
		tex->name = getNameFromPath(tex->exported_file.c_str(), true);

		ilBindImage(0);
		ilDeleteImage(image);
		ret = true;
	}

	return ret;
}

void ModuleResourceLoader::ProcessTexture(uint& texture) {	
	texture = ilutGLBindTexImage();

	if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		LOG("Error converting image: %s", iluErrorString(ilGetError()));

	long h, w, bpp, f;
	ILubyte *texdata = 0;

	w = ilGetInteger(IL_IMAGE_WIDTH);
	h = ilGetInteger(IL_IMAGE_HEIGHT);
	bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	f = ilGetInteger(IL_IMAGE_FORMAT);
	texdata = ilGetData();

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, GL_MAX_TEXTURE_MAX_ANISOTROPY);
	gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, w, h, f, GL_UNSIGNED_BYTE, texdata);
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

void ModuleResourceLoader::LoadEngineUI() {
	icon_tex = (ResourceTexture*)App->res_manager->CreateNewResource(RESOURCE_TYPE::TEXTURE, engineIcon_UUID);
	model_icon_tex = (ResourceTexture*)App->res_manager->CreateNewResource(RESOURCE_TYPE::TEXTURE, modelIcon_UUID);
	scene_icon_tex = (ResourceTexture*)App->res_manager->CreateNewResource(RESOURCE_TYPE::TEXTURE, sceneIcon_UUID);
	tex_icon_tex = (ResourceTexture*)App->res_manager->CreateNewResource(RESOURCE_TYPE::TEXTURE, texIcon_UUID);

	icon_tex->file = "None";
	model_icon_tex->file = "None";
	scene_icon_tex->file = "None";
	tex_icon_tex->file = "None";

	icon_tex->exported_file = "library/textures/icon.dds";
	model_icon_tex->exported_file = "library/textures/model_icon.dds";
	scene_icon_tex->exported_file = "library/textures/scene_icon.dds";
	tex_icon_tex->exported_file = "library/textures/texture_icon.dds";

	LoadTextureFromLibrary(icon_tex);
	LoadTextureFromLibrary(model_icon_tex);
	LoadTextureFromLibrary(scene_icon_tex);
	LoadTextureFromLibrary(tex_icon_tex);
}

void ModuleResourceLoader::CreateDefaultMaterial() {
	CreateDefaultTexture();

	default_material = (ResourceTexture*)App->res_manager->CreateNewResource(RESOURCE_TYPE::TEXTURE, defaultMat_UUID);
	default_material->texture = default_tex;
	default_material->name = "Default texture";
	default_material->file = "None";
	default_material->exported_file = "None";
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

// Save & Load
void ModuleResourceLoader::Load(const nlohmann::json &config) {
	defaultMat_UUID = config["Resource loader"]["Default material UUID"];
	engineIcon_UUID = config["Resource loader"]["Engine icon UUID"];
	modelIcon_UUID = config["Resource loader"]["Model icon UUID"];
	texIcon_UUID = config["Resource loader"]["Texture icon UUID"];
	sceneIcon_UUID = config["Resource loader"]["Scene icon UUID"];
}

void ModuleResourceLoader::Save(nlohmann::json &config) {
	config["Resource loader"]["Default material UUID"] = defaultMat_UUID;
	config["Resource loader"]["Engine icon UUID"] = engineIcon_UUID;
	config["Resource loader"]["Model icon UUID"] = modelIcon_UUID;
	config["Resource loader"]["Texture icon UUID"] = texIcon_UUID;
	config["Resource loader"]["Scene icon UUID"] = sceneIcon_UUID;
}
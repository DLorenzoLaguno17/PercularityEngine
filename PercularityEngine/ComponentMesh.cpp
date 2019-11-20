#include "ComponentMesh.h"
#include "ImGui/imgui.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleFileSystem.h"
#include "ModuleRenderer3D.h"
#include "ComponentTransform.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

#include "Par Shapes/par_shapes.h"
#include "mmgr/mmgr.h"

ComponentMesh::ComponentMesh(GameObject* parent, bool active) :
	Component(COMPONENT_TYPE::MESH, parent, active) {}

void ComponentMesh::Update() 
{
	Render();	
	RenderNormals();
}

void ComponentMesh::OnEditor() {

	if (ImGui::CollapsingHeader("Mesh")) {
		ImGui::Checkbox("Enabled", &active);

		ImGui::Text("Vertices:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%d", resource_mesh->num_vertices);

		ImGui::Text("Polygons:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%d", resource_mesh->num_indices / 3);
		
		ImGui::NewLine();
		ImGui::Text("Show normals");
		ImGui::Checkbox("Vertex normals", &showVertexNormals);
		ImGui::Checkbox("Face normals", &showFaceNormals);

		ImGui::NewLine();
	}
}

void ComponentMesh::CleanUp()
{
	resource_mesh->CleanUp();
}

// Method to render vertex normals
void ComponentMesh::RenderNormals() {

	if (resource_mesh->normals != nullptr && showVertexNormals) {

		for (uint i = 0; i < resource_mesh->num_vertices; ++i) {
			float3 point = resource_mesh->vertices[i];
			float3 vec = resource_mesh->normals[i];

			glLineWidth(1.0f);
			glBegin(GL_LINES);
			glColor3f(0, 255, 255);
			glVertex3f(point.x, point.y, point.z);
			glVertex3f((point.x + vec.x * NORMALS_LENGTH),
				(point.y + vec.y * NORMALS_LENGTH),
				(point.z + vec.z * NORMALS_LENGTH));

			glColor3f(255, 255, 255);
			glEnd();
		}
	}

	if (showFaceNormals) {
		for (uint i = 0; i < resource_mesh->num_vertices; i += 3)
		{
			float3 a = resource_mesh->vertices[resource_mesh->indices[i]];
			float3 b = resource_mesh->vertices[resource_mesh->indices[i + 1]];
			float3 c = resource_mesh->vertices[resource_mesh->indices[i + 2]];

			float3 vec = Cross((b - a), (c - a));
			vec.Normalize();
			float3 center = float3((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3);

			glLineWidth(1.0f);
			glBegin(GL_LINES);
			glColor3f(255, 255, 0);
			glVertex3f(center.x, center.y, center.z);
			glVertex3f((center.x + vec.x * NORMALS_LENGTH),
				(center.y + vec.y * NORMALS_LENGTH),
				(center.z + vec.z * NORMALS_LENGTH));

			glColor3f(255, 255, 255);
			glEnd();
		}
	}
}

void ComponentMesh::Render() const  {
	
	glPushMatrix();
	glMultMatrixf(gameObject->transform->renderTransform.M);

	// Render the texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	ComponentMaterial* texture = gameObject->GetComponent<ComponentMaterial>();
	
	if (texture->IsActive()) 
		glBindTexture(GL_TEXTURE_2D, texture->resource_tex->texture);

	else glBindTexture(GL_TEXTURE_2D, App->res_loader->default_tex);

	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->id_UVs);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	// Render the mesh
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource_mesh->id_index);
	glDrawElements(GL_TRIANGLES, resource_mesh->num_indices, GL_UNSIGNED_INT, NULL);

	// Clean all buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void ComponentMesh::LoadParShape(par_shapes_mesh_s* parShape, const char* primitiveType)
{	
	// Load the vertices
	resource_mesh->num_vertices = parShape->npoints;
	resource_mesh->vertices = new float3[resource_mesh->num_vertices];

	for (uint i = 0; i < resource_mesh->num_vertices; ++i)
	{
		resource_mesh->vertices[i].x = parShape->points[i * 3];
		resource_mesh->vertices[i].y = parShape->points[i * 3 + 1];
		resource_mesh->vertices[i].z = parShape->points[i * 3 + 2];
	}

	resource_mesh->id_vertex = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float3) * resource_mesh->num_vertices, resource_mesh->vertices);

	// Load the indices
	resource_mesh->num_indices = parShape->ntriangles * 3;
	resource_mesh->indices = new uint[resource_mesh->num_indices];

	for (uint i = 0; i < resource_mesh->num_indices; ++i)
		resource_mesh->indices[i] = parShape->triangles[i];

	resource_mesh->id_index = App->renderer3D->CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * resource_mesh->num_indices, resource_mesh->indices);
	
	// Load texture coords
	resource_mesh->num_UVs = resource_mesh->num_vertices * 2;
	resource_mesh->coords = new float[resource_mesh->num_UVs];

	for (uint i = 0; i < resource_mesh->num_vertices; ++i)
	{
		resource_mesh->coords[i * 2] = parShape->tcoords[i * 2];
		resource_mesh->coords[i * 2 + 1] = parShape->tcoords[i * 2 + 1];
	}

	resource_mesh->id_UVs = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float) * resource_mesh->num_UVs, resource_mesh->coords);

	// Load normals
	if (parShape->normals)
	{
		resource_mesh->normals = new float3[parShape->npoints];

		for (int i = 0; i < parShape->npoints; ++i)
		{
			resource_mesh->normals[i].x = parShape->normals[i * 3];
			resource_mesh->normals[i].y = parShape->normals[i * 3 + 1];
			resource_mesh->normals[i].z = parShape->normals[i * 3 + 2];
		}
	}

	resource_mesh->name = primitiveType;
}

// Load & Save 
void ComponentMesh::OnLoad(const char* gameObjectNum, const nlohmann::json &scene_file) {
	json js = scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["Name"];
	///mesh_name = js.get<std::string>();
	
	UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["UUID"];
	parent_UUID = scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["Parent UUID"];
	active = scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["Active"];
	showFaceNormals = scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["F_Normals on"];
	showVertexNormals = scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["V_Normals on"];
	
	///std::string path = LIBRARY_MESH_FOLDER + mesh_name + ".mesh";
	///App->res_loader->LoadMeshFromLibrary(path.c_str(), resource_mesh);
	aabb.SetNegativeInfinity();
	aabb = AABB::MinimalEnclosingAABB(resource_mesh->vertices, resource_mesh->num_vertices);
}

void ComponentMesh::OnSave(const char* gameObjectNum, nlohmann::json &scene_file) {
	scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["UUID"] = UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["Parent UUID"] = parent_UUID;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["Active"] = active;
	///scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["Name"] = mesh_name;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["F_Normals on"] = showFaceNormals;
	scene_file["Game Objects"][gameObjectNum]["Components"]["Mesh"]["V_Normals on"] = showVertexNormals;
}
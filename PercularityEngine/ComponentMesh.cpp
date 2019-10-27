#include "ComponentMesh.h"
#include "imgui.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "ModuleRenderer3D.h"


#include "Par Shapes/par_shapes.h"

#include "mmgr/mmgr.h"

void ComponentMesh::Update() {
	
	Render();	
	RenderNormals();
}

void ComponentMesh::OnEditor() {
	if (ImGui::CollapsingHeader("Mesh")) {
		ImGui::Checkbox("Enabled", &active);

		ImGui::Text("Vertices:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%d", mesh.num_vertices);

		ImGui::Text("Polygons:");
		ImGui::SameLine();
		ImGui::TextColored({ 255, 255, 0, 255 }, "%d", mesh.num_indices / 3);
		
		ImGui::NewLine();
		ImGui::Text("Show normals");
		ImGui::Checkbox("Vertex normals", &showVertexNormals);
		ImGui::Checkbox("Face normals", &showFaceNormals);

		ImGui::NewLine();
	}
}

// Method to render vertex normals
void ComponentMesh::RenderNormals() {

	if (mesh.normals != nullptr && showVertexNormals) {

		for (uint i = 0; i < mesh.num_vertices; ++i) {
			float3 point = mesh.vertices[i];
			float3 vec = mesh.normals[i];

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
		for (uint i = 0; i < mesh.num_vertices; i += 3)
		{
			float3 a = mesh.vertices[mesh.indices[i]];
			float3 b = mesh.vertices[mesh.indices[i + 1]];
			float3 c = mesh.vertices[mesh.indices[i + 2]];

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
	
	// Render the texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	if (parent->c_texture->active) glBindTexture(GL_TEXTURE_2D, parent->c_texture->texture);
	else glBindTexture(GL_TEXTURE_2D, App->res_loader->default_tex);

	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.id_tex);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	// Render the mesh
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,mesh.id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glDrawElements(GL_TRIANGLES, mesh.num_indices * 3, GL_UNSIGNED_INT, NULL);

	// Clean all buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ComponentMesh::LoadParShape(par_shapes_mesh_s* parShape)
{
	
	mesh.num_indices = parShape->ntriangles * 3;
	mesh.num_vertices = parShape->npoints;

	//Load the vertices
	mesh.vertices = new float3[mesh.num_vertices];

	for (uint i = 0; i < mesh.num_vertices; ++i)
	{
		mesh.vertices[i].x = parShape->points[3 * i];
		mesh.vertices[i].y = parShape->points[(3 * i)+1];
		mesh.vertices[i].z = parShape->points[(3 * i)+2];
	}

	mesh.id_vertex = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float)*mesh.num_vertices*3, mesh.vertices);

	//Load the indices
	mesh.indices = new uint[mesh.num_indices];

	for (uint i = 0; i < mesh.num_indices; ++i)
		mesh.indices[i] = parShape->triangles[i];

	mesh.id_index = App->renderer3D->CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*mesh.num_indices , mesh.indices);
	
	//Load texture coords
	mesh.num_tex = mesh.num_vertices * 2;
	mesh.textures = new float[mesh.num_tex];

	for (uint i = 0; i < mesh.num_vertices; ++i)
	{
		mesh.textures[2 * i] = parShape->tcoords[2 * i];
		mesh.textures[(2 * i)+1] = parShape->tcoords[(2 * i) + 1];
	}

	mesh.id_tex = App->renderer3D->CreateBuffer(GL_ARRAY_BUFFER, sizeof(float)*mesh.num_tex, mesh.textures);

	//Load normals
	if (parShape->normals)
	{
		mesh.normals = new float3[parShape->npoints];

		for (uint i = 0; i < parShape->npoints; ++i)
		{
			mesh.normals[i].x =parShape->normals[3 * i];
			mesh.normals[i].y = parShape->normals[(3 * i)+1];
			mesh.normals[i].z = parShape->normals[(3 * i)+2];
		}
	}

	CreateBoundingBox();

}

void ComponentMesh::CreateBoundingBox()
{
	float minX, minY, minZ, maxX, maxY,maxZ;

	for (uint i = 0; i < mesh.num_vertices; ++i)
	{
		if (i == 0)
		{
			minX = mesh.vertices[i].x;
			maxX = mesh.vertices[i].x;
			minY = mesh.vertices[i].y;
			maxY = mesh.vertices[i].y;
			minZ = mesh.vertices[i].z;
			maxZ = mesh.vertices[i].z;
		}
			
		else
		{
			if (mesh.vertices[i].x > maxX)
				maxX = mesh.vertices[i].x;
			else if (mesh.vertices[i].x < minX)
				minX = mesh.vertices[i].x;

			if (mesh.vertices[i].y > maxY)
				maxY = mesh.vertices[i].y;
			else if (mesh.vertices[i].y < minY)
				minY = mesh.vertices[i].y;

			if (mesh.vertices[i].z > maxZ)
				maxZ = mesh.vertices[i].z;
			else if (mesh.vertices[i].z < minZ)
				minZ = mesh.vertices[i].z;
		}
	}

	parent->boundingBox.maxX = maxX;
	parent->boundingBox.minX = minX;
	parent->boundingBox.maxY = maxY;
	parent->boundingBox.minY = minY;
	parent->boundingBox.maxZ = maxZ;
	parent->boundingBox.minZ = minZ;

	parent->boundingBox.box[0] = {maxX, maxY, maxZ};
	parent->boundingBox.box[1] = { maxX, maxY, minZ };
	parent->boundingBox.box[2] = { maxX, minY, maxZ };
	parent->boundingBox.box[3] = { maxX, minY, minZ };
	parent->boundingBox.box[4] = { minX, maxY, maxZ };
	parent->boundingBox.box[5] = { minX, maxY, minZ };
	parent->boundingBox.box[6] = { minX, minY, maxZ };
	parent->boundingBox.box[7] = { minX, minY, minZ };

}
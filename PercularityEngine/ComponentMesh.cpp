#include "ComponentMesh.h"
#include "imgui.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleResourceLoader.h"

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
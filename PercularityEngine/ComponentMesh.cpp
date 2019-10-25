#include "ComponentMesh.h"
#include "imgui.h"
#include "OpenGL.h"
#include "glmath.h"

void ComponentMesh::Update() {
	if (showVertexNormals) RenderVertexNormals();
	if (showFaceNormals) RenderFaceNormals();
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
void ComponentMesh::RenderVertexNormals() {

	if (mesh.normals != nullptr) {

		for (uint i = 0; i < mesh.num_vertices; ++i) {
			vec3 point = vec3(mesh.vertices[i * 3], mesh.vertices[i * 3 + 1], mesh.vertices[i * 3 + 2]);
			vec3 vec = vec3(mesh.normals[i * 3], mesh.normals[i * 3 + 1], mesh.normals[i * 3 + 2]);

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
}

void ComponentMesh::RenderFaceNormals() {

	for (uint i = 0; i < mesh.num_indices; ++i)
	{
		vec3 a = vec3(mesh.vertices[i * 3], mesh.vertices[i * 3 + 1], mesh.vertices[i * 3 + 2]);
		vec3 b = vec3(mesh.vertices[i * 3 + 3], mesh.vertices[i * 3 + 4], mesh.vertices[i * 3 + 5]);
		vec3 c = vec3(mesh.vertices[i * 3 + 6], mesh.vertices[i * 3 + 7], mesh.vertices[i * 3 + 8]);
	
		vec3 vec = cross((b - a), (c - a));
	
		vec3 face_center = vec3(
			(a.x + b.x + c.x) / 3,
			(a.y + b.y + c.y) / 3,
			(a.z + b.z + c.z) / 3
		);
	
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glColor3f(0, 255, 0);
		glVertex3f(face_center.x, face_center.y, face_center.z);
		glVertex3f((face_center.x + vec.x * NORMALS_LENGTH),
			(face_center.y + vec.y * NORMALS_LENGTH),
			(face_center.z + vec.z * NORMALS_LENGTH));

		glColor3f(255, 255, 255);
		glEnd();
	}
}
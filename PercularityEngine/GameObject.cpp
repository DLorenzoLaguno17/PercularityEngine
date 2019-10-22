#include "GameObject.h"
#include "OpenGL.h"
#include "glmath.h"

void GameObject::Render() const {
	for (uint i = 0; i < mesh.size(); ++i) {

		// Render the texture
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, mesh[i]->id_tex);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		// Render the mesh
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh[i]->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh[i]->id_index);
		glDrawElements(GL_TRIANGLES, mesh[i]->num_indices * 3, GL_UNSIGNED_INT, NULL);

		// Clean all buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void GameObject::CreateComponent(COMPONENT_TYPE type) {
	Component* ret = nullptr;
	/*switch (type)
	{
	case COMPONENT_TYPE::TEXTURE:
		ret = new j1Player(type);
		if (ret != nullptr) components.push_back(ret); break;

	case COMPONENT_TYPE::MESH:
		ret = new j1Hook(type);
		if (ret != nullptr) components.push_back(ret); break;
	}*/
}

void GameObject::CleanUp() {

	for (uint i = 0; i < mesh.size(); ++i)
		mesh[i]->CleanUp();

	mesh.clear();
}

void GameObject::ShowNormals() {

	for (int i = 0; i < mesh.size(); ++i) {

		if (mesh[i]->normals != nullptr) {

			// Vertex normals
			for (uint j = 0; j < mesh[i]->num_vertices; ++j) {
				vec3 point = vec3(mesh[i]->vertices[j * 3], mesh[i]->vertices[j * 3 + 1], mesh[i]->vertices[j * 3 + 2]);
				vec3 vec = vec3(mesh[i]->normals[j * 3], mesh[i]->normals[j * 3 + 1], mesh[i]->normals[j * 3 + 2]);

				glBegin(GL_LINES);
				glColor3f(0, 255, 255);
				glLineWidth(1.0f);
				glVertex3f(point.x, point.y, point.z);
				glVertex3f((point.x + vec.x * NORMALS_LENGTH),
					(point.y + vec.y * NORMALS_LENGTH),
					(point.z + vec.z * NORMALS_LENGTH));

				glEnd();
			}

			// Face normals
			/*for (uint j = 0; j < fbx_mesh.mesh[i]->num_vertices; j += 3)
			{
				vec3 a = vec3(fbx_mesh.mesh[i]->vertices[j * 3], fbx_mesh.mesh[i]->vertices[j * 3 + 1], fbx_mesh.mesh[i]->vertices[j * 3 + 2]);
				vec3 b = vec3(fbx_mesh.mesh[i]->vertices[j * 3], fbx_mesh.mesh[i]->vertices[j * 3 + 1], fbx_mesh.mesh[i]->vertices[j * 3 + 2]);
				vec3 c = vec3(fbx_mesh.mesh[i]->vertices[j * 3], fbx_mesh.mesh[i]->vertices[j * 3 + 1], fbx_mesh.mesh[i]->vertices[j * 3 + 2]);

				vec3 vec = cross((b - a), (c - a));

				vec3 face_center = vec3(
					(a.x + b.x + c.x) / 3,
					(a.y + b.y + c.y) / 3,
					(a.z + b.z + c.z) / 3
				);

				glBegin(GL_LINES);
				glColor3f(0, 255, 0);
				glVertex3f(face_center.x, face_center.y, face_center.z);
				glVertex3f((face_center.x + vec.x * NORMALS_LENGTH),
					(face_center.y + vec.y * NORMALS_LENGTH),
					(face_center.z + vec.z * NORMALS_LENGTH));

				glEnd();
			}*/
		}
	}
}
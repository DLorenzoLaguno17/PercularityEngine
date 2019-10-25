#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
//#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "OpenGL.h"
#include "glmath.h"

#include "mmgr/mmgr.h"

GameObject::GameObject(std::string name, GameObject* parent) :
	name(name), parent(parent)
{
	c_mesh = (ComponentMesh*)CreateComponent(COMPONENT_TYPE::MESH);
	c_texture = (ComponentMaterial*)CreateComponent(COMPONENT_TYPE::MATERIAL);
}

// Called every frame
void GameObject::Update() {
	for (int i = 0; i < components.size(); ++i)
		if (components[i]->active) components[i]->Update();
}

// Creates components for its GameObject
Component* GameObject::CreateComponent(COMPONENT_TYPE type, bool active) {

	Component* ret = nullptr;

	switch (type)
	{
	case COMPONENT_TYPE::MATERIAL:
		ret = new ComponentMaterial(type, this, active);
		if (ret != nullptr) components.push_back(ret); break;

	case COMPONENT_TYPE::MESH:
		ret = new ComponentMesh(type, this, active);
		if (ret != nullptr) components.push_back(ret); break;

	/*case COMPONENT_TYPE::TRANSFORM
		ret = new ComponentMesh(type, this, active);
		if (ret != nullptr) components.push_back(ret); break;*/
	}

	return ret;
}

void GameObject::OnEditor() {
	for (int i = 0; i < components.size(); ++i)
		components[i]->OnEditor();
}

// Cleans the memory of the GameObject
void GameObject::CleanUp() {
	for (int i = 0; i < components.size(); ++i) {
		delete components[i];
		components[i] = nullptr;
	}
}

void GameObject::Render() const {
	// Render the texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (c_texture->active) glBindTexture(GL_TEXTURE_2D, c_texture->texture);
	else glBindTexture(GL_TEXTURE_2D, App->res_loader->default_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, c_mesh->mesh.id_tex);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	// Render the mesh
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, c_mesh->mesh.id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c_mesh->mesh.id_index);
	glDrawElements(GL_TRIANGLES, c_mesh->mesh.num_indices * 3, GL_UNSIGNED_INT, NULL);

	// Clean all buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleResourceLoader.h"
#include "imgui.h"

#include"OpenGL.h"
#include "DevIL/include/ilut.h":

ComponentMaterial::ComponentMaterial(COMPONENT_TYPE type, GameObject* parent, bool active) : 
	Component(type, parent, active) {}

void ComponentMaterial::Update() {

}

void ComponentMaterial::CreateTexture(const char* path) {
	ILuint image;
	GLuint tex;
	ilGenImages(1, &image);
	ilBindImage(image);

	if (!ilLoadImage(path)) {
		ilDeleteImages(1, &image);
		tex_name = "Default texture";
		LOG("The texture image could not be loaded")
			texture = App->res_loader->default_tex;
	}
	else {
		tex = ilutGLBindTexImage();
		LOG("Created texture from path: %s", path)
		LOG("")

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

		// Storing texture data
		width = w;
		height = h;

		std::string p = path;
		if (strstr(path, "Assets/Textures/"))
			tex_name = p.substr(p.find_last_of("//") + 1);
		else
			tex_name = App->res_loader->getNameFromPath(p, true);

		texture = tex;
	}
}

void ComponentMaterial::OnEditor() {

	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Checkbox(tex_name.c_str(), &active);
		ImGui::Text("Size: %dx%d", height, width);
		ImGui::NewLine();

		ImGui::Image((void*)texture, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::NewLine();
	}
}
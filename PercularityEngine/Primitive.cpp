#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Primitive.h"


#define PAR_SHAPES_IMPLEMENTATION

#include "Par Shapes/par_shapes.h"


Primitive::Primitive()
{}

Primitive::~Primitive()
{
	delete mesh;
}

void Primitive::Render() const 
{
	//Bind frame & render buffers
	glBindFramebuffer(GL_FRAMEBUFFER, App->renderer3D->GetFrameBuffer());
	glBindRenderbuffer(GL_FRAMEBUFFER, App->renderer3D->GetRenderBuffer());

	//Enable client state
	glEnableClientState(GL_VERTEX_ARRAY);

	//Bind vertices & indices buffers
	glBindBuffer(GL_ARRAY_BUFFER, verticesId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
	glDrawElements(GL_TRIANGLES, mesh->ntriangles*3, GL_UNSIGNED_SHORT, nullptr);

	//Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Disable client state
	glDisableClientState(GL_VERTEX_ARRAY);

	//Unbind frame & render buffers
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Primitive::StoreData()
{
	if (mesh != nullptr)
	{
		glGenBuffers(1, (GLuint*) &(verticesId));
		glBindBuffer(GL_ARRAY_BUFFER, verticesId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->npoints *3, mesh->points, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		
		glGenBuffers(1, (GLuint*) &(indicesId));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PAR_SHAPES_T)*mesh->ntriangles*3, mesh->triangles, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

//~~~~~~~~SPHERE~~~~~~~~

Sphere::Sphere()
{

}

Sphere::Sphere(int slices, int stacks)
{
	mesh = par_shapes_create_parametric_sphere(slices, stacks);

	StoreData();
}

Sphere::~Sphere()
{

}

//~~~~~~~~CUBE~~~~~~~~~~

Cube::Cube()
{
	mesh = par_shapes_create_cube();

	StoreData();
}

Cube::~Cube()
{

}
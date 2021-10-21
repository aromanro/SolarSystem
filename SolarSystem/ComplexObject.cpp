#include "stdafx.h"
#include "ComplexObject.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace OpenGL {

	ComplexObject::ComplexObject(const ObjLoader& loader)
		: VertexBufferRenderable()
	{
		if (loader.triangles.size() == 0) return;

		const long long int STRIDE_SIZE = 6; // 3 for vertex, 3 for normal, maybe later 3 for tangent - the one with texture will add 2 for texture coordinate

		vertexCount = static_cast<int>(3 * loader.triangles.size()); // 3 vertexes / triangle
		GLfloat *vertices = new GLfloat[STRIDE_SIZE * vertexCount];
		
		
		int baseIndex = 0;
		for (const auto& triangle : loader.triangles)
		{
			vertices[baseIndex] = static_cast<GLfloat>(triangle->A.X); 
			vertices[baseIndex + 1] = static_cast<GLfloat>(triangle->A.Y);
			vertices[baseIndex + 2] = static_cast<GLfloat>(triangle->A.Z);
			vertices[baseIndex + 3] = static_cast<GLfloat>(triangle->normal1.X);
			vertices[baseIndex + 4] = static_cast<GLfloat>(triangle->normal1.Y);
			vertices[baseIndex + 5] = static_cast<GLfloat>(triangle->normal1.Z);
			
			vertices[baseIndex + 6] = static_cast<GLfloat>(triangle->B.X);
			vertices[baseIndex + 7] = static_cast<GLfloat>(triangle->B.Y);
			vertices[baseIndex + 8] = static_cast<GLfloat>(triangle->B.Z);
			vertices[baseIndex + 9] = static_cast<GLfloat>(triangle->normal2.X);
			vertices[baseIndex + 10] = static_cast<GLfloat>(triangle->normal2.Y);
			vertices[baseIndex + 11] = static_cast<GLfloat>(triangle->normal2.Z);

			vertices[baseIndex + 12] = static_cast<GLfloat>(triangle->C.X);
			vertices[baseIndex + 13] = static_cast<GLfloat>(triangle->C.Y);
			vertices[baseIndex + 14] = static_cast<GLfloat>(triangle->C.Z);
			vertices[baseIndex + 15] = static_cast<GLfloat>(triangle->normal3.X);
			vertices[baseIndex + 16] = static_cast<GLfloat>(triangle->normal3.Y);
			vertices[baseIndex + 17] = static_cast<GLfloat>(triangle->normal3.Z);

			baseIndex += 3 * STRIDE_SIZE;
		}

		Renderable::Bind();

		setData(vertices, vertexCount * sizeof(GLfloat) * STRIDE_SIZE);
		// Link vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		delete[] vertices;
	}

	void ComplexObject::Draw()
	{
		VertexBufferRenderable::Bind();

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		//VertexBufferRenderable::UnBind();
	}

	void ComplexObject::DrawInstanced(unsigned int count)
	{
		VertexBufferRenderable::Bind();

		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, count);
	}


}


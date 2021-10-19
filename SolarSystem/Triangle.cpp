#include "stdafx.h"
#include "Triangle.h"


namespace OpenGL {

	Triangle::Triangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c)
		: A(a), B(b), C(c), U1(-1), V1(-1), U2(-1), V2(-1), U3(-1), V3(-1), threeNormals(false), useInterpolation(false)
	{
		Init();
	}


	Triangle::Triangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c, const Vector3D<double>& n1, const Vector3D<double>& n2, const Vector3D<double>& n3)
		: A(a), B(b), C(c), U1(-1), V1(-1), U2(-1), V2(-1), U3(-1), V3(-1), threeNormals(true),
		normal1(n1), normal2(n2), normal3(n3), useInterpolation(false)
	{
		Init();
	}

	void Triangle::Init()
	{
		edge1 = B - A;
		edge2 = C - A;
		normal = edge1 % edge2;
		area = normal.Length() * 0.5;
		normal = normal.Normalize();

		if (!threeNormals)
			normal1 = normal2 = normal3 = normal;
	}

	void OpenGLTriangle::Draw()
	{
		VertexBufferRenderable::Bind();

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//VertexBufferRenderable::UnBind();
	}

	void OpenGLTriangle::DrawInstanced(unsigned int count)
	{
		VertexBufferRenderable::Bind();

		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, count);
	}



	void OpenGLTriangle::Init()
	{
		//Triangle::Init();

		const int STRIDE_SIZE = 6; // 3 for vertex, 3 for normal, maybe later 3 for tangent - the one with texture will add 2 for texture coordinate
		GLfloat vertices[] = {
			static_cast<GLfloat>(A.X), static_cast<GLfloat>(A.Y), static_cast<GLfloat>(A.Z), static_cast<GLfloat>(normal1.X), static_cast<GLfloat>(normal1.Y), static_cast<GLfloat>(normal1.Z),
			static_cast<GLfloat>(B.X), static_cast<GLfloat>(B.Y), static_cast<GLfloat>(B.Z), static_cast<GLfloat>(normal2.X), static_cast<GLfloat>(normal2.Y), static_cast<GLfloat>(normal2.Z),
			static_cast<GLfloat>(C.X), static_cast<GLfloat>(C.Y), static_cast<GLfloat>(C.Z), static_cast<GLfloat>(normal3.X), static_cast<GLfloat>(normal3.Y), static_cast<GLfloat>(normal3.Z)
		};

		Renderable::Bind();

		setData(vertices, sizeof(vertices));
		// Link vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}
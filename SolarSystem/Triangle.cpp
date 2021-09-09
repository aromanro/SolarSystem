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

		// TODO: Implement the OpenGL part
	}

	void Triangle::Draw()
	{
		VertexBufferRenderable::Bind();

		
		// TODO: Implement it!

		//VertexBufferRenderable::UnBind();
	}

	void Triangle::DrawInstanced(unsigned int count)
	{
		VertexBufferRenderable::Bind();

		// TODO: Implement it!
	}

}
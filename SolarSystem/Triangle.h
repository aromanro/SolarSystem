#pragma once

#include "Vector3D.h"
#include "Color.h"

#include <tuple>


#include "VertexBufferRenderable.h"

namespace OpenGL {


	class Triangle
	{
	public:
		Triangle() : area(0), U1(-1), V1(-1), U2(-1), V2(-1), U3(-1), V3(-1), useInterpolation(true), threeNormals(true) {}
		Triangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c);
		Triangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c, const Vector3D<double>& n1, const Vector3D<double>& n2, const Vector3D<double>& n3);
		virtual ~Triangle() {}

		void Translate(const Vector3D<double>& t)
		{
			A += t;
			B += t;
			C += t;
		}

		void RotateAround(const Vector3D<double>& v, double angle)
		{
			A = A.RotateAround(v, angle);
			B = B.RotateAround(v, angle);
			C = C.RotateAround(v, angle);

			edge1 = B - A;
			edge2 = C - A;
			// area does not change

			normal = (edge1 % edge2).Normalize();

			if (threeNormals)
			{
				normal1 = normal1.RotateAround(v, angle);
				normal2 = normal2.RotateAround(v, angle);
				normal3 = normal3.RotateAround(v, angle);
			}
		}

		void Scale(double scale)
		{
			A *= scale;
			B *= scale;
			C *= scale;
			edge1 *= scale;
			edge2 *= scale;

			area *= scale * scale;
		}

		inline static double Interpolate(double u, double v, double val1, double val2, double val3)
		{
			const double w = 1. - u - v;

			return w * val1 + u * val2 + v * val3;
		}


		inline static std::pair<double, double> Interpolate(double u, double v, const std::pair<double, double> v1, const std::pair<double, double> v2, const std::pair<double, double> v3)
		{
			const double w = 1. - u - v;

			return std::make_pair(w * v1.first + u * v2.first + v * v3.first, w * v1.second + u * v2.second + v * v3.second);
		}

		inline static Color Interpolate(double u, double v, const Color& col1, const Color& col2, const Color& col3)
		{
			const double w = 1. - u - v;

			return w * col1 + u * col2 + v * col3;
		}


		void SetUseInterpolation()
		{
			if (U1 >= 0 && V1 >= 0 && U2 >= 0 && V2 >= 0 && U3 >= 0 && V3 >= 0)
				useInterpolation = true;
			else
				useInterpolation = false;
		}

		inline const Vector3D<double> getNormal(double u, double v) const
		{
			if (threeNormals)
			{
				const double w = 1. - u - v;

				return (w * normal1 + u * normal2 + v * normal3).Normalize();
			}

			return normal;
		}

		Vector3D<double> A, B, C;

		double U1, V1, U2, V2, U3, V3;

	protected:
		void Init();

		bool threeNormals;

		double area;
		Vector3D<double> normal; // the computed normal, it's the real one

		// normals supplied by other means, for each vertex (for example by loading from an obj file or programmatically)
		// they are not necessarily the same as the normal above
		// they can be used to 'fake' a curved triangle, by computing a normal in the hit point using them, based on barycentric coordinates
		Vector3D<double> normal1;
		Vector3D<double> normal2;
		Vector3D<double> normal3;

		Vector3D<double> edge1;
		Vector3D<double> edge2;

		bool useInterpolation;
	};


	class OpenGLTriangle : public VertexBufferRenderable, public Triangle
	{
	public:
		OpenGLTriangle() : VertexBufferRenderable(), Triangle() {}
		OpenGLTriangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c)
			: VertexBufferRenderable(), Triangle(a, b, c)
		{
			Init();
		}

		OpenGLTriangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c, const Vector3D<double>& n1, const Vector3D<double>& n2, const Vector3D<double>& n3)
			: VertexBufferRenderable(), Triangle(a, b, c, n1, n2, n3)
		{
			Init();
		}

		virtual ~OpenGLTriangle() {};


		virtual void Draw() override;
		void DrawInstanced(unsigned int count);

	protected:
		void Init();
	};

}
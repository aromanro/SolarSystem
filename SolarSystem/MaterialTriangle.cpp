#include "stdafx.h"
#include "MaterialTriangle.h"


namespace OpenGL {


	MaterialTriangle::MaterialTriangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c, const Vector3D<double>& n1, const Vector3D<double>& n2, const Vector3D<double>& n3, const Material& mat)
		: Triangle(a, b, c, n1, n2, n3), material(mat)
	{
	}

}


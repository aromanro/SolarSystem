#pragma once

#include "Vector3D.h"
#include "Material.h"
#include "Triangle.h"


namespace OpenGL {


    class MaterialTriangle :
        public Triangle
    {
    public:
        MaterialTriangle(const Vector3D<double>& a, const Vector3D<double>& b, const Vector3D<double>& c, const Vector3D<double>& n1, const Vector3D<double>& n2, const Vector3D<double>& n3, const ObjMaterial& mat);

        ObjMaterial material;
    };

}



#pragma once

#include "MaterialTriangle.h"

#include "Material.h"

#include <memory>
#include <vector>
#include <tuple>
#include <map>

class ObjLoader
{
public:
	typedef std::vector<std::tuple<size_t, long long int, long long int>> Polygon;

	ObjLoader();
	~ObjLoader();

	static void LeftTrim(std::string& str);

	bool Load(const std::string& name, bool center = true);

	bool LoadMaterial(const std::string& name, const std::string& dir);

	static bool IsConcaveVertex(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, int pointIndex, double& sine);
	static bool IsConcave(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, int& pointIndex);

	// TODO: add rotate, scale, translate...
	void Translate(const Vector3D<double>& t)
	{
		for (auto& obj : triangles)
			obj->Translate(t);
	}

	void RotateAround(const Vector3D<double>& v, double angle)
	{
		for (auto& obj : triangles)
			obj->RotateAround(v, angle);
	}


	void Scale(double s)
	{
		for (auto& obj : triangles)
			obj->Scale(s);
	}

	std::string dir;

	std::map<std::string, ObjMaterial> materials;
	std::vector<std::shared_ptr<OpenGL::MaterialTriangle>> triangles;

protected:
	void SetTriangles(const std::vector<std::pair<double, double>>& textureCoords, const std::vector<Vector3D<double>>& normals, const std::vector<Vector3D<double>>& vertices, const std::vector<std::pair<Polygon, std::string>>& polygons);
};


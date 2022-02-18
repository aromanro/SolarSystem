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
	typedef std::vector<std::tuple<size_t, size_t, size_t>> Polygon;

	ObjLoader();
	~ObjLoader();

	static void LeftTrim(std::string& str);

	bool Load(const std::string& name, bool center = true);

	bool LoadMaterial(const std::string& name, const std::string& dir);

	static bool IsConcaveVertex(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, int pointIndex, double& sine);
	static bool IsConcave(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, int& pointIndex);

	static bool AddTriangle(int ind1, int ind2, int ind3, const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, const std::vector<Vector3D<double>>& normals, const std::vector<std::pair<double, double>>& textureCoords, const Material& material, std::vector<std::shared_ptr<OpenGL::Triangle>>& triangles);

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

	std::map<std::string, Material> materials;
	std::vector<std::shared_ptr<OpenGL::MaterialTriangle>> triangles;
};


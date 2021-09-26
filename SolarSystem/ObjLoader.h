#pragma once

#include "Triangle.h"

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

	static bool AddTriangle(int ind1, int ind2, int ind3, const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, const std::vector<Vector3D<double>>& normals, const std::vector<std::pair<double, double>>& textureCoords/*, std::shared_ptr<Materials::Material> material*/, std::vector<std::shared_ptr<OpenGL::Triangle>>& triangles);

	std::map<std::string, Material> materials;
	std::vector<std::shared_ptr<OpenGL::Triangle>> triangles;
};


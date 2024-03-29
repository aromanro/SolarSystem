#pragma once

#include "MaterialTriangle.h"

#include "Material.h"

#include <memory>
#include <vector>
#include <tuple>
#include <map>
#include <unordered_map>

class ObjLoader
{
public:
	using Polygon = std::vector<std::tuple<size_t, long long int, long long int>>;

	bool Load(const std::string& name, bool center = true);

	void Translate(const Vector3D<double>& t) const
	{
		for (auto& obj : triangles)
			obj->Translate(t);
	}

	void RotateAround(const Vector3D<double>& v, double angle) const
	{
		for (auto& obj : triangles)
			obj->RotateAround(v, angle);
	}

	void Scale(double s) const
	{
		for (auto& obj : triangles)
			obj->Scale(s);
	}

	std::string dir;

	std::unordered_map<std::string, ObjMaterial> materials;
	std::vector<std::shared_ptr<OpenGL::MaterialTriangle>> triangles;

private:
	bool LoadMaterial(const std::string& name, const std::string& dir);

	static bool IsConcaveVertex(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, int pointIndex, double& sine);
	static bool IsConcave(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, int& pointIndex);

	static void LeftTrim(std::string& str);

	static void LoadVertexInfo(std::string& line, std::vector<Vector3D<double>>& vertices, std::vector<Vector3D<double>>& normals, std::vector<std::pair<double, double>>& textureCoords, Vector3D<double>& centerCoord);
	static void LoadFace(std::string& line, const std::string& curMaterial, std::vector<std::pair<Polygon, std::string>>& polygons, const std::vector<Vector3D<double>>& vertices, const std::vector<Vector3D<double>>& normals, const std::vector<std::pair<double, double>>& textureCoords);

	void SetTriangles(const std::vector<std::pair<double, double>>& textureCoords, const std::vector<Vector3D<double>>& normals, const std::vector<Vector3D<double>>& vertices, const std::vector<std::pair<Polygon, std::string>>& polygons);
	void AddTriangle(const Vector3D<double>& firstPoint, const Vector3D<double>& secondPoint, const Vector3D<double>& lastPoint, const Vector3D<double>& firstNormal, const Vector3D<double>& secondNormal, const Vector3D<double>& lastNormal, ObjMaterial& material, const std::vector<std::pair<double, double>>& textureCoords, long long int firstIndexTex, long long int indextex2, long long int lastIndexTex);
	void AddTriangleNoNormals(const Vector3D<double>& firstPoint, const Vector3D<double>& secondPoint, const Vector3D<double>& lastPoint, ObjMaterial& material, const std::vector<std::pair<double, double>>& textureCoords, long long int firstIndexTex, long long int indextex2, long long int lastIndexTex);
	bool SplitPolygon(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, const std::vector<Vector3D<double>>& normals, const std::vector<std::pair<double, double>>& textureCoords, const std::string& matName, ObjMaterial& whiteMaterial);
	bool SplitPolygonNoNormals(const Polygon& polygon, const std::vector<Vector3D<double>>& vertices, const std::vector<std::pair<double, double>>& textureCoords, const std::string& matName, ObjMaterial& whiteMaterial);
};


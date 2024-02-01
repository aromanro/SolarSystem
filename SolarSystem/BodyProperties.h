#pragma once

#include "Texture.h"

#include <memory>
#include <map>

class BodyProperties
{
public:
	bool isSun = false;
	bool isMoon = false;
	COLORREF color = 0;

	double tilt = 0.;

	double scale = 1.;
	double scaleDistance = 1.;
	
	CString imgFile;
	CString transparentFile;
	CString shadowFile;
	CString specularFile;
	CString normalFile; // might be a bump mapping file, with one gray channel, in this case use Sobel to compute the normals

	std::shared_ptr<OpenGL::Texture> texture;
	std::shared_ptr<OpenGL::Texture> transparentTexture;
	std::shared_ptr<OpenGL::Texture> shadowTexture;
	std::shared_ptr<OpenGL::Texture> specularTexture;
	std::shared_ptr<OpenGL::Texture> normalTexture;

	bool transparentTextureAlpha = false;

	double bumpParam = 2.;

	int parentIndex = -1; // just an index for the body where the moon belongs

	bool LoadTexture();
	void CleanTexture();

	static std::shared_ptr<OpenGL::Texture> LoadTexture(const CString& name, int bindNo = 0, int bpp = 24);
	static std::shared_ptr<OpenGL::Texture> LoadNormalTexture(const CString& name, double bumpParam, int bindNo);

	static std::shared_ptr<CImage> Load(const CString& name);

	static void ClearTexturesCache();

private:
	static double GetPixelValue(const CImage& img, int x, int y);
	static void ResizeToEven(CImage& skin);

	static std::shared_ptr<OpenGL::Texture> CachedTexture(const CString& path, int bindNo, int nrBytes);

	static std::map<CString, std::shared_ptr<CImage>> texturesMap;
	static std::map<std::tuple<CString, int, int>, std::shared_ptr<OpenGL::Texture>> glTexturesMap;
};


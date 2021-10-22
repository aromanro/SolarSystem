#pragma once

#include "Texture.h"

class BodyProperties
{
public:
	bool isSun;
	bool isMoon;
	COLORREF color;


	double tilt;

	double scale;
	double scaleDistance;
	
	CString imgFile;
	CString transparentFile;
	CString shadowFile;
	CString specularFile;
	CString normalFile; // might be a bump mapping file, with one gray channel, in this case use Sobel to compute the normals

	OpenGL::Texture* texture;
	OpenGL::Texture* transparentTexture;
	OpenGL::Texture* shadowTexture;
	OpenGL::Texture* specularTexture;
	OpenGL::Texture* normalTexture;

	bool transparentTextureAlpha;

	double bumpParam;

	int parentIndex; // just an index for the body where the moon belongs

	BodyProperties();
	~BodyProperties();
	bool LoadTexture();
	void CleanTexture();

	static OpenGL::Texture* LoadTexture(const CString& name, int bindNo = 0, int bpp = 24);
	static OpenGL::Texture* LoadNormalTexture(const CString& name, double bumpParam, int bindNo);

protected:
	static double GetPixelValue(const CImage& img, int x, int y);
};


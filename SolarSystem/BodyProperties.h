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

	OpenGL::Texture* texture;
	OpenGL::Texture* transparentTexture;
	OpenGL::Texture* shadowTexture;
	OpenGL::Texture* specularTexture;

	BodyProperties();
	~BodyProperties();
	bool LoadTexture();
	void CleanTexture();
};


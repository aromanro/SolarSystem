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

	OpenGL::Texture *texture;

	BodyProperties();
	~BodyProperties();
	bool LoadTexture();
	void CleanTexture();
};


#pragma once
class Options
{
public:
	bool gammaCorrection;
	bool showSkyBox;
	bool drawShadows;
	bool drawTextures;

	unsigned int translationSpeed;
	unsigned int rotationSpeed;
	unsigned int scrollSpeed;

	Options();
	~Options();
	bool Load();
	bool Save();
};


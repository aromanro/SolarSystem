#pragma once

class Options
{
public:
	int gammaCorrection;
	bool showSkyBox;
	bool drawShadows;
	bool drawTextures;

	unsigned int translationSpeed;
	unsigned int rotationSpeed;
	unsigned int scrollSpeed;

	bool showBillboard;

	double gammaCorrectionCoeff;

	// spaceship related
	CString spaceshipObjFile;

	double translate; // how much to translate it to be in front of the camera
	double scale; // how much to scale it to have it to the desired size

	// how much to rotate it to have it in the desired orientation in relation with the camera
	double rotateX;
	double rotateY;
	double rotateZ;


	Options();
	~Options();
	bool Load();
	bool Save();

private:
	static double GetDouble(LPCTSTR param, double defval);
};


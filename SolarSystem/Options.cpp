#include "stdafx.h"
#include "Options.h"

#include "SolarSystem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Options::Options()
	: gammaCorrection(false), showSkyBox(true), drawShadows(true), drawTextures(true),
	translationSpeed(100), rotationSpeed(100), scrollSpeed(100), showBillboard(false),
	gammaCorrectionCoeff(1.),
	translate(0.2), scale(0.01), rotateX(0), rotateY(0), rotateZ(0)
{
}


Options::~Options()
{
}


bool Options::Load()
{
	int res = static_cast<int>(theApp.GetProfileInt(L"options", L"gamma", 0));
	gammaCorrection = (res != 0 ? true : false);

	res = static_cast<int>(theApp.GetProfileInt(L"options", L"skyBox", 1));
	showSkyBox = (res != 0 ? true : false);

	res = static_cast<int>(theApp.GetProfileInt(L"options", L"shadows", 1));
	drawShadows = (res != 0 ? true : false);

	res = static_cast<int>(theApp.GetProfileInt(L"options", L"textures", 1));
	drawTextures = (res != 0 ? true : false);

	translationSpeed = theApp.GetProfileInt(L"options", L"translationSpeed", 100);
	rotationSpeed = theApp.GetProfileInt(L"options", L"rotationSpeed", 100);
	scrollSpeed = theApp.GetProfileInt(L"options", L"scrollSpeed", 100);

	res = static_cast<int>(theApp.GetProfileInt(L"options", L"showBillboard", 0));
	showBillboard = (res != 0 ? true : false);

	gammaCorrectionCoeff = GetDouble(L"gammaCorrectionCoeff", 1.);

	spaceshipObjFile = theApp.GetProfileString(L"options", L"spaceship");

	translate = GetDouble(L"translate", 0.2);
	scale = GetDouble(L"scale", 0.01);
	rotateX = GetDouble(L"rotateX", 0);
	rotateY = GetDouble(L"rotateY", 0);
	rotateZ = GetDouble(L"rotateZ", 0);

	return true;
}


bool Options::Save()
{
	theApp.WriteProfileInt(L"options", L"gamma", gammaCorrection ? 1 : 0);
	theApp.WriteProfileInt(L"options", L"skyBox", showSkyBox ? 1 : 0);
	theApp.WriteProfileInt(L"options", L"shadows", drawShadows ? 1 : 0);
	theApp.WriteProfileInt(L"options", L"textures", drawTextures ? 1 : 0);

	theApp.WriteProfileInt(L"options", L"translationSpeed", (int)translationSpeed);
	theApp.WriteProfileInt(L"options", L"rotationSpeed", (int)rotationSpeed);
	theApp.WriteProfileInt(L"options", L"scrollSpeed", (int)scrollSpeed);

	theApp.WriteProfileInt(L"options", L"showBillboard", showBillboard ? 1 : 0);

	theApp.WriteProfileBinary(L"options", L"gammaCorrectionCoeff", (LPBYTE)&gammaCorrectionCoeff, sizeof(double));

	theApp.WriteProfileString(L"options", L"spaceship", spaceshipObjFile);

	theApp.WriteProfileBinary(L"options", L"translate", (LPBYTE)&translate, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"scale", (LPBYTE)&scale, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"rotateX", (LPBYTE)&rotateX, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"rotateY", (LPBYTE)&rotateY, sizeof(double));
	theApp.WriteProfileBinary(L"options", L"rotateZ", (LPBYTE)&rotateZ, sizeof(double));

	return false;
}

double Options::GetDouble(LPCTSTR param, double defval)
{
	double val = defval;

	UINT sz = 0;
	LPBYTE buf = NULL;

	if (theApp.GetProfileBinary(L"options", param, &buf, &sz))
	{
		if (sizeof(double) == sz) val = *((double*)buf);
		delete[] buf;
	}

	return val;
}

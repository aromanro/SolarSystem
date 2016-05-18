#include "stdafx.h"
#include "SolarSystemBodies.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SolarSystemBodies::SolarSystemBodies()
{
}


SolarSystemBodies::~SolarSystemBodies()
{
}


void SolarSystemBodies::clear()
{
	m_Bodies.clear();
	m_BodyProperties.clear();
}


void SolarSystemBodies::LoadTextures()
{
	for (auto &body : m_BodyProperties)
	{
		if (body.LoadTexture())
			body.texture->GenerateMipmaps();
	}
}

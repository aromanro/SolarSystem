#include "stdafx.h"
#include "SolarSystemBodies.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SolarSystemBodies::SolarSystemBodies()
	: m_simulationTime(0)
{
}


SolarSystemBodies::~SolarSystemBodies()
{
}


void SolarSystemBodies::clear()
{
	m_Bodies.clear();
	m_BodyProperties.clear();
	m_simulationTime = 0;
}


void SolarSystemBodies::LoadTextures()
{
	for (auto &body : m_BodyProperties)
	{
		if (body.LoadTexture())
			body.texture->GenerateMipmaps();
	}
}

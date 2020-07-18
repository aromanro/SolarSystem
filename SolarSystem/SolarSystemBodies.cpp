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
		body.LoadTexture();
		if (body.texture)
		{
			body.texture->Bind();
			body.texture->GenerateMipmaps();
			body.texture->UnBind();
		}
		if (body.transparentTexture)
		{
			body.transparentTexture->Bind(1);
			body.transparentTexture->GenerateMipmaps();
			body.transparentTexture->UnBind();
		}
		if (body.shadowTexture)
		{
			body.shadowTexture->Bind(2);
			body.shadowTexture->GenerateMipmaps();
			body.shadowTexture->UnBind();
		}
		if (body.specularTexture)
		{
			body.specularTexture->Bind(3);
			body.specularTexture->GenerateMipmaps();
			body.specularTexture->UnBind();
		}			
		if (body.normalTexture)
		{
			body.normalTexture->Bind(4);
			body.normalTexture->GenerateMipmaps();
			body.normalTexture->UnBind();
		}
	}
}

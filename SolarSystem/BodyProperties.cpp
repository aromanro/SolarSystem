#include "stdafx.h"
#include "BodyProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BodyProperties::BodyProperties()
	: isSun(false), color(0), texture(NULL), isMoon(false), tilt(0), scale(1.), scaleDistance(1.)
{
}


BodyProperties::~BodyProperties()
{
	delete texture;
}


bool BodyProperties::LoadTexture()
{
	CleanTexture();
	
	if (!imgFile.IsEmpty())
	{
		try {
			CImage skin;
			skin.Load(imgFile);

			if (skin.IsNull()) return false;
			
			texture = new OpenGL::Texture();
			unsigned char* buf;

			if (skin.GetPitch() < 0)
				buf = (unsigned char*)skin.GetPixelAddress(0, skin.GetHeight() - 1);
			else 
				buf = (unsigned char*)skin.GetBits();

			texture->setData(buf, skin.GetWidth(), skin.GetHeight());

			return true;
		}
		catch (...)
		{
			CleanTexture();
		}
	}

	return false;
}


void BodyProperties::CleanTexture()
{
	delete texture;
	texture = NULL;
}
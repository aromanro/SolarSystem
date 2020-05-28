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
			else if (skin.GetBPP() != 24) return false;

			// ideally they should be power of 2 but even values should do

			// check if it's even
			unsigned int dim = skin.GetWidth();
			if (dim % 2) return false;

			// check if it's even
			dim = skin.GetHeight();
			if (dim % 2) return false;

			texture = new OpenGL::Texture();
			unsigned char* buf = NULL;

			if (skin.GetPitch() < 0)
				buf = static_cast<unsigned char*>(skin.GetPixelAddress(0, skin.GetHeight() - 1));
			else 
				buf = static_cast<unsigned char*>(skin.GetBits());

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
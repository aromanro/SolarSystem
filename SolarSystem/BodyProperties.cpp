#include "stdafx.h"
#include "BodyProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BodyProperties::BodyProperties()
	: isSun(false), isMoon(false), color(0), tilt(0), scale(1.), scaleDistance(1.), texture(NULL), transparentTexture(NULL), shadowTexture(NULL)
{
}


BodyProperties::~BodyProperties()
{
	delete transparentTexture;
	delete shadowTexture;
	delete texture;
}


bool BodyProperties::LoadTexture()
{
	CleanTexture();
	
	if (!transparentFile.IsEmpty())
	{
		try {
			CImage skin;
			skin.Load(transparentFile);

			if (!(skin.IsNull() || skin.GetBPP() != 24))
			{

				// ideally they should be power of 2 but even values should do

				// check if it's even
				unsigned int dim = skin.GetWidth();
				if (!(dim % 2))
				{

					// check if it's even
					dim = skin.GetHeight();
					if (!(dim % 2))
					{

						transparentTexture = new OpenGL::Texture();
						unsigned char* buf = NULL;

						if (skin.GetPitch() < 0)
							buf = static_cast<unsigned char*>(skin.GetPixelAddress(0, skin.GetHeight() - 1));
						else
							buf = static_cast<unsigned char*>(skin.GetBits());

						transparentTexture->setData(buf, skin.GetWidth(), skin.GetHeight(), 1);
					}
				}
			}
		}
		catch (...)
		{
			delete transparentTexture;
			transparentTexture = NULL;
		}
	}


	if (!shadowFile.IsEmpty())
	{
		try {
			CImage skin;
			skin.Load(shadowFile);

			if (!(skin.IsNull() || skin.GetBPP() != 24))
			{

				// ideally they should be power of 2 but even values should do

				// check if it's even
				unsigned int dim = skin.GetWidth();
				if (!(dim % 2))
				{

					// check if it's even
					dim = skin.GetHeight();
					if (!(dim % 2))
					{

						shadowTexture = new OpenGL::Texture();
						unsigned char* buf = NULL;

						if (skin.GetPitch() < 0)
							buf = static_cast<unsigned char*>(skin.GetPixelAddress(0, skin.GetHeight() - 1));
						else
							buf = static_cast<unsigned char*>(skin.GetBits());

						shadowTexture->setData(buf, skin.GetWidth(), skin.GetHeight(), 2);
					}
				}
			}
		}
		catch (...)
		{
			delete shadowTexture;
			shadowTexture = NULL;
		}
	}


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

	delete transparentTexture;
	transparentTexture = NULL;

	delete shadowTexture;
	shadowTexture = NULL;
}
#include "stdafx.h"
#include "BodyProperties.h"
#include "MemoryBitmap.h"

#include <glm.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BodyProperties::BodyProperties()
	: isSun(false), isMoon(false), color(0), tilt(0), scale(1.), scaleDistance(1.), texture(NULL), transparentTexture(NULL), shadowTexture(NULL), specularTexture(NULL), normalTexture(NULL), transparentTextureAlpha(false), parentIndex(-1)
{
}


BodyProperties::~BodyProperties()
{
	delete normalTexture;
	delete specularTexture;
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

			if (!(skin.IsNull() || (skin.GetBPP() != 24 && skin.GetBPP() != 32)))
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

						transparentTextureAlpha = skin.GetBPP() == 32;
						transparentTexture->setData(buf, skin.GetWidth(), skin.GetHeight(), 1, transparentTextureAlpha ? 4 : 3);
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


	if (!specularFile.IsEmpty())
	{
		try {
			CImage skin;
			skin.Load(specularFile);

			if (!(skin.IsNull() || skin.GetBPP() != 8))
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

						specularTexture = new OpenGL::Texture();
						unsigned char* buf = NULL;

						if (skin.GetPitch() < 0)
							buf = static_cast<unsigned char*>(skin.GetPixelAddress(0, skin.GetHeight() - 1));
						else
							buf = static_cast<unsigned char*>(skin.GetBits());

						specularTexture->setData(buf, skin.GetWidth(), skin.GetHeight(), 3, 1);
					}
				}
			}
		}
		catch (...)
		{
			delete specularTexture;
			specularTexture = NULL;
		}
	}


	if (!normalFile.IsEmpty())
	{
		try {
			CImage skin;
			skin.Load(normalFile);

			if (!skin.IsNull() && (skin.GetBPP() == 8 || skin.GetBPP() == 24))
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
						normalTexture = new OpenGL::Texture();
						unsigned char* buf = NULL;

						if (skin.GetBPP() == 24)
						{
							if (skin.GetPitch() < 0)
								buf = static_cast<unsigned char*>(skin.GetPixelAddress(0, skin.GetHeight() - 1));
							else
								buf = static_cast<unsigned char*>(skin.GetBits());

							normalTexture->setData(buf, skin.GetWidth(), skin.GetHeight(), 3, 1);
						}
						else
						{
							// if it's a bump map, there is some work to do
							MemoryBitmap memoryBitmap(skin.GetWidth(), skin.GetHeight());

							// TODO: set the normal map into the memory bitmap (using Sobel to convert from bump map)

							for (int row = 0; row < skin.GetHeight(); ++row)
							{
								for (int col = 0; col < skin.GetWidth(); ++col)
								{
									const double topLeft = GetPixelValue(skin, row - 1, col - 1);
									const double left = GetPixelValue(skin, row, col - 1);
									const double bottomLeft = GetPixelValue(skin, row + 1, col - 1);
									const double topRight = GetPixelValue(skin, row - 1, col + 1);
									const double right = GetPixelValue(skin, row, col + 1);
									const double bottomRight = GetPixelValue(skin, row + 1, col + 1);
									const double top = GetPixelValue(skin, row - 1, col);
									const double bottom = GetPixelValue(skin, row + 1, col);

									// Sobel (actually, -dx, -dy)
									const double mdX = topRight - topLeft + 2. * (right - left) + bottomRight - bottomLeft;
									const double mdY = bottomLeft - topLeft + 2. * (bottom - top) + bottomRight - topRight;
									
									const double dZ = 1.;

									glm::vec3 v(mdX, mdY, dZ);
									v = glm::normalize(v);
									// now convert to RGB
									const double R = 255. * 0.5 * (1. + v.x);
									const double G = 255. * 0.5 * (1. + v.y);
									const double B = 255. * 0.5 * (1. + v.z);

									memoryBitmap.SetPixel(row, col, RGB(static_cast<unsigned char>(R), static_cast<unsigned char>(G), static_cast<unsigned char>(B)));
								}
							}

							memoryBitmap.SetIntoTexture(*normalTexture);
						}
					}
				}
			}
		}
		catch (...)
		{
			delete normalTexture;
			normalTexture = NULL;
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


double BodyProperties::GetPixelValue(const CImage& img, int x, int y)
{
	const int width = img.GetWidth();
	const int height = img.GetHeight();

	if (x < 0) x += width;
	else if (x >= width) x -= width;
	
	if (y < 0) x += height;
	else if (y >= height) y -= height;

	const unsigned char* paddr = static_cast<const unsigned char*>(img.GetPixelAddress(x, y));
	if (paddr) return *paddr / 255.;

	return 0;
}


void BodyProperties::CleanTexture()
{
	delete texture;
	texture = NULL;

	delete transparentTexture;
	transparentTexture = NULL;

	delete shadowTexture;
	shadowTexture = NULL;

	delete normalTexture;
	normalTexture = NULL;

	transparentTextureAlpha = false;
}
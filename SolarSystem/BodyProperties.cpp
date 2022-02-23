#include "stdafx.h"
#include "BodyProperties.h"
#include "MemoryBitmap.h"

#include <filesystem>

#include <glm.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::map<CString, std::shared_ptr<CImage>> BodyProperties::texturesMap = {};
std::map<std::tuple<CString, int, int>, std::shared_ptr<OpenGL::Texture>> BodyProperties::glTexturesMap = {};


BodyProperties::BodyProperties()
	: isSun(false), isMoon(false), color(0), tilt(0), scale(1.), scaleDistance(1.), texture(NULL), transparentTexture(NULL), shadowTexture(NULL), specularTexture(NULL), normalTexture(NULL), transparentTextureAlpha(false), bumpParam(2.), parentIndex(-1)
{
}


BodyProperties::~BodyProperties()
{
}


bool BodyProperties::LoadTexture()
{
	CleanTexture();

	if (!transparentFile.IsEmpty())
	{
		try {
			transparentTexture = CachedTexture(transparentFile, 0, 3);

			auto skin = Load(transparentFile);
			if (skin && !((skin->GetBPP() != 24 && skin->GetBPP() != 32)))
			{
				// ideally they should be power of 2 but even values should do
				// 'Load' should resize the image to even, the checks are legacy but I'll let them here
				 
				// check if it's even
				unsigned int dim = skin->GetWidth();
				if (!(dim % 2))
				{
					// check if it's even
					dim = skin->GetHeight();
					if (!(dim % 2))
					{
						transparentTexture = std::make_shared<OpenGL::Texture>();
						unsigned char* buf = NULL;

						if (skin->GetPitch() < 0)
							buf = static_cast<unsigned char*>(skin->GetPixelAddress(0, skin->GetHeight() - 1));
						else
							buf = static_cast<unsigned char*>(skin->GetBits());

						transparentTextureAlpha = skin->GetBPP() == 32;
						transparentTexture->setData(buf, skin->GetWidth(), skin->GetHeight(), 1, transparentTextureAlpha ? 4 : 3);

						glTexturesMap[std::make_tuple(transparentFile, 0, 3)] = transparentTexture;
					}
				}
			}
		}
		catch (...)
		{
			transparentTexture.reset();
		}
	}

	shadowTexture = LoadTexture(shadowFile, 2, 24);
	specularTexture = LoadTexture(specularFile, 3, 8);
	normalTexture = LoadNormalTexture(normalFile, bumpParam, 4);

	texture = LoadTexture(imgFile);
	if (texture) return true;

	return false;
}

// the textures are clamped, they are fitted around a sphere, whence the wrap around for negative values or for the ones that overflow
double BodyProperties::GetPixelValue(const CImage& img, int x, int y)
{
	const int width = img.GetWidth();
	const int height = img.GetHeight();

	if (x < 0) x += width;
	else if (x >= width) x -= width;

	if (y < 0) y += height;
	else if (y >= height) y -= height;

	const unsigned char* paddr = static_cast<const unsigned char*>(img.GetPixelAddress(x, y));
	if (paddr) return *paddr / 255.;

	// this is very slow!
	//COLORREF rgb = img.GetPixel(x, y);
	//return ((double)GetRValue(rgb) + (double)GetGValue(rgb) + (double)GetBValue(rgb)) / (3. * 255.);

	return 0.;
}


void BodyProperties::CleanTexture()
{
	texture.reset();
	transparentTexture.reset();
	shadowTexture.reset();
	specularTexture.reset();
	normalTexture.reset();

	transparentTextureAlpha = false;
}

void BodyProperties::ResizeToEven(CImage& skin)
{
	// resize it if needed
	if (skin.GetWidth() % 2 || skin.GetHeight() % 2)
	{
		int iNewWidth = skin.GetWidth();
		if (iNewWidth % 2) ++iNewWidth;
		int iNewHeight = skin.GetHeight();
		if (iNewHeight % 2) ++iNewHeight;

		CImage newImage;
		newImage.Create(iNewWidth, iNewHeight, skin.GetBPP());
		SetStretchBltMode(newImage.GetDC(), COLORONCOLOR);

		skin.StretchBlt(newImage.GetDC(), 0, 0, iNewWidth, iNewHeight, 0, 0, skin.GetWidth(), skin.GetHeight(), SRCCOPY);

		// need to call it twice to actually release it, because of a pesky counter inside
		newImage.ReleaseDC();
		newImage.ReleaseDC();

		skin.Destroy();
		skin.Attach(newImage.Detach());
	}
}


std::shared_ptr<OpenGL::Texture> BodyProperties::LoadTexture(const CString& imgFile, int bindNo, int bpp)
{
	const int nrBytes = bpp / 8;
	std::shared_ptr<OpenGL::Texture> texture = CachedTexture(imgFile, bindNo, nrBytes);

	if (!imgFile.IsEmpty())
	{
		try {
			auto skin = Load(imgFile);
			if (!skin || skin->GetBPP() != bpp) return NULL;

			// ideally they should be power of 2 but even values should do

			// 'Load' should resize the image to even, the checks are legacy but I'll let them here

			// check if it's even
			unsigned int dim = skin->GetWidth();
			if (dim % 2) return NULL;

			// check if it's even
			dim = skin->GetHeight();
			if (dim % 2) return NULL;

			texture = std::make_shared<OpenGL::Texture>();
			unsigned char* buf = NULL;

			if (skin->GetPitch() < 0)
				buf = static_cast<unsigned char*>(skin->GetPixelAddress(0, skin->GetHeight() - 1));
			else
				buf = static_cast<unsigned char*>(skin->GetBits());

			texture->setData(buf, skin->GetWidth(), skin->GetHeight(), bindNo, nrBytes);

			glTexturesMap[std::make_tuple(imgFile, bindNo, nrBytes)] = texture;
		}
		catch (...)
		{
			texture.reset();
		}
	}

	return texture;
}

std::shared_ptr<OpenGL::Texture> BodyProperties::LoadNormalTexture(const CString& normalFile, double bumpParam, int bindNo)
{
	std::shared_ptr<OpenGL::Texture> normalTexture = CachedTexture(normalFile, bindNo, 3);

	if (!normalFile.IsEmpty())
	{
		try {
			auto skin = Load(normalFile);

			if (skin && (skin->GetBPP() == 8 || skin->GetBPP() == 24))
			{
				// ideally they should be power of 2 but even values should do

				// 'Load' should resize the image to even, the checks are legacy but I'll let them here

				// check if it's even
				unsigned int dim = skin->GetWidth();
				if (!(dim % 2))
				{
					// check if it's even
					dim = skin->GetHeight();
					if (!(dim % 2))
					{
						normalTexture = std::make_shared<OpenGL::Texture>();
						unsigned char* buf = NULL;

						if (skin->GetBPP() == 24)
						{
							if (skin->GetPitch() < 0)
								buf = static_cast<unsigned char*>(skin->GetPixelAddress(0, skin->GetHeight() - 1));
							else
								buf = static_cast<unsigned char*>(skin->GetBits());

							normalTexture->setData(buf, skin->GetWidth(), skin->GetHeight(), bindNo);
						}
						else
						{
							// if it's a bump map, there is some work to do
							MemoryBitmap memoryBitmap(skin->GetWidth(), skin->GetHeight());

							// TODO: set the normal map into the memory bitmap (using Sobel to convert from bump map)

							for (int y = 0; y < skin->GetHeight(); ++y)
							{
								for (int x = 0; x < skin->GetWidth(); ++x)
								{
									const double top = GetPixelValue(*skin, x, y + 1);
									const double bottom = GetPixelValue(*skin, x, y - 1);
									const double left = GetPixelValue(*skin, x - 1, y);
									const double right = GetPixelValue(*skin, x + 1, y);

									const double topLeft = GetPixelValue(*skin, x - 1, y + 1);
									const double bottomLeft = GetPixelValue(*skin, x - 1, y - 1);
									const double topRight = GetPixelValue(*skin, x + 1, y + 1);
									const double bottomRight = GetPixelValue(*skin, x + 1, y - 1);

									// Sobel
									const double dX = topRight - topLeft + 2. * (right - left) + bottomRight - bottomLeft;
									const double dY = bottomLeft - topLeft + 2. * (bottom - top) + bottomRight - topRight;

									const double dZ = bumpParam; // make it smaller to increase the slope 

									glm::vec3 v(-dX, -dY, dZ);
									v = glm::normalize(v);

									// now convert to RGB
									const double R = 255. * 0.5 * (1. + v.x);
									const double G = 255. * 0.5 * (1. + v.y);
									const double B = 255. * 0.5 * (1. + v.z);

									memoryBitmap.SetPixel(x, y, RGB(static_cast<unsigned char>(R), static_cast<unsigned char>(G), static_cast<unsigned char>(B)));
								}
							}

							//memoryBitmap.Save(CString("C:\\temp\\") + normalFile);

							memoryBitmap.SetIntoTexture(*normalTexture, bindNo);
						}

						glTexturesMap[std::make_tuple(normalFile, bindNo, 3)] = normalTexture;
					}
				}
			}
		}
		catch (...)
		{
			normalTexture.reset();
		}
	}

	return normalTexture;
}


void BodyProperties::ClearTexturesCache()
{
	texturesMap.clear();
	glTexturesMap.clear();
}

std::shared_ptr<CImage> BodyProperties::Load(const CString& name)
{
	try {
		if (texturesMap.find(name) != texturesMap.end())
			return texturesMap.at(name);

		CImage* skin = new CImage();
		
		if (FAILED(skin->Load(name)) || skin->IsNull())
		{
			delete skin;
			return {};
		}

		/*
		bool save = false;
		if (skin->GetWidth() % 2 || skin->GetHeight() % 2)
			save = true;
		*/

		ResizeToEven(*skin);

		/*
		if (save)
		{
			std::wstring s((LPCTSTR)name);
			std::filesystem::path dirName(s);

			std::string str = std::string("c:\\temp\\Img\\") + dirName.filename().string();

			skin->Save(CString(str.c_str()));
		}
		*/

		std::shared_ptr<CImage> sp(skin);
		texturesMap.emplace(name, sp);

		return sp;
	}
	catch (...)
	{
	}

	return {};
}

std::shared_ptr<OpenGL::Texture> BodyProperties::CachedTexture(const CString& path, int bindNo, int nrBytes)
{
	if (path.IsEmpty()) return {};

	auto index = std::make_tuple(path, bindNo, nrBytes);
	if (glTexturesMap.find(index) != glTexturesMap.end())
		return glTexturesMap.at(index);

	return {};
}
